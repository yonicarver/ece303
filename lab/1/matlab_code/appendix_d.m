% Resistor Estimation Lab-USB
close all;clear all;clc

DMM=visa('agilent','USB0::0x2A8D::0xB318::MY58170025::0::INSTR');
awg=visa('agilent','USB0::0x0957::0x0407::MY44043469::0::INSTR');
scope=visa('agilent','USB0::0x0957::0x1799::MY58100813::0::INSTR');

% Inputs
pulse_frequency=1400;
pulse_amplitude=5;
dutycycle=[20:5:80,75:-5:20];
volts_per_division=2;
time_range='3E-3';

scope.InputBufferSize=2^17;
scope.OutputBufferSize=2^17;

% Open instruments
fopen(awg);
fopen(scope);

% Set up equipment
fprintf(awg,'OUTP:LOAD INF');
offset=pulse_amplitude/2;
freq_str=num2str(pulse_frequency);
amp_str=num2str(pulse_amplitude);
offset_str=num2str(offset);
cmd_str=['APPL:SQU ' freq_str ',' amp_str ',' offset_str];
fprintf(awg,cmd_str);
range_str=[':CHANNEL1:RANGE ' num2str(8*volts_per_division) '\n'];
fprintf(scope,range_str);
fprintf(scope,[':TIMEBASE:MODE NORMAL; RANGE ' time_range]);
fprintf(scope,[':AUTOSCALE;']);
f=zeros(2000,length(dutycycle));
data=zeros(2000,length(dutycycle));
tv=query(scope,':CHANNEL1:RANGE?;');
offset=query(scope,':CHANNEL1:OFFSET?');

dat=zeros(2000,length(dutycycle));

operationComplete = str2double(query(scope,'*OPC?'));

fprintf(scope,'WAV:SOUR CHAN1;');
fprintf(scope,'WAV:FORM WORD;');
fprintf(scope,'WAV:POINTS:MODE RAW');
fprintf(scope,'WAV:POIN 2000');
fprintf(scope,':WAVEFORM:BYTEORDER LSBFirst');
for I=1:length(dutycycle)
	fprintf(awg,'PULSE:DCYC %i',dutycycle(I));
	fprintf(scope,'DIG CHAN1;');
	operationComplete = str2double(query(scope,'*OPC?'));
	while ~operationComplete
		operationComplete = str2double(query(scope,'*OPC?'));
	end
	preambleBlock = query(scope,':WAVEFORM:PREAMBLE?');
	fprintf(scope,'WAV:DATA?;');
	data(:,I)=binblockread(scope,'uint16'); fread(scope,1);
	f(I)=str2double(query(scope,':MEAS:FREQUENCY?'));
end

% Convert dat from binary representation to actual voltage
tv_num=str2double(tv);
offset_num=str2double(offset);
preamble=str2double(strsplit(preambleBlock,','));
dt=preamble(5);
t0=preamble(6);
t_ref=preamble(7);
dy=preamble(8);
y0=preamble(9);
y_ref=preamble(10);

t=dt*(1:2000)-dt;
data_volts=(dy.*(data-y_ref))+y0;

fclose(awg);
fclose(scope);
delete(awg);
delete(scope);
figure
plot(t,data_volts)

%%

counter_array = zeros(1, 25);
for i=1:25
    for j=287:length(data_volts)
        if data_volts(j,i) > 4
            counter_array(i) = counter_array(i) + 1;
        else
            break;
        end
    end
end  

%%
trendline = polyfit(dutycycle,counter_array,1);
x = linspace(20, 80, 600);
y = trendline(1)*x + trendline(2);

figure
plot(dutycycle, counter_array, '*', 'Linewidth', 2)
hold on
plot(x, y)
xlabel('Duty Cycle (%)')
ylabel('On-Time (us)')
title('Duty Cycle vs. On-Time')
legend('Measured On-Time', 'Trendline')
grid on