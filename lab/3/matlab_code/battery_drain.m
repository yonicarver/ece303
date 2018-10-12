%% Draining a Battery-USB
close all;clear all;clc

% Inputs
volts_per_division=2;
time_range='3E-3';

voltage_reading = zeros(1, 7200);
current_reading = zeros(1, 7200);

% Create equipment connections and objects
DMM=visa('agilent','USB0::0x2A8D::0xB318::MY58170025::0::INSTR');
scope=visa('agilent','USB0::0x0957::0x1799::MY58100813::0::INSTR');



% Open instruments
fopen(DMM);
fopen(scope);

scope.InputBufferSize=2^17;
scope.OutputBufferSize=2^17;

range_str=[':CHANNEL1:RANGE ' num2str(8*volts_per_division) '\n'];
fprintf(scope,range_str);
fprintf(scope,[':TIMEBASE:MODE NORMAL; RANGE ' time_range]);
fprintf(scope,[':AUTOSCALE;']);

fprintf(scope,'WAV:SOUR CHAN1;');
fprintf(scope,'WAV:FORM WORD;');
fprintf(scope,'WAV:POINTS:MODE RAW');
fprintf(scope,'WAV:POIN 2000');
fprintf(scope,':WAVEFORM:BYTEORDER LSBFirst');

i = 1;
tic
while (i)
	pause(i-toc)
 
        fprintf(scope,'DIG CHAN1;');
	operationComplete = str2double(query(scope,'*OPC?'));
	while ~operationComplete
		operationComplete = str2double(query(scope,'*OPC?'));
	end
	preambleBlock = query(scope,':WAVEFORM:PREAMBLE?');
	fprintf(scope,'WAV:DATA?;');
	voltage_reading(i) = binblockread(scope,'uint16');		% appends voltage reading
	disp(voltage_reading(i));
        fread(scope,1);
	current_reading(i) = str2double(query(DMM,'MEAS:CURR:DC?')); % Measure Current
	i = i+1;
        if (voltage_reading(i) < 4.8)
            break;
        end
end

while (1)
    if (voltage_reading(end) == 0)
        voltage_reading(end) = [];
        current_reading(end) = [];
    else
        break;
    end
end

% Close and delete instrument connections and objects
fclose(DMM);
fclose(scope);

delete(DMM)
delete(scope);




%%

trendline = polyfit(V,Imeas*1000,1);
x = linspace(0, 10, 100);
y = trendline(1)*x + trendline(2);

%figure
%plot(V,Imeas*1000,'o','MarkerFaceColor','b')
%xlabel('Voltage (V)')
%ylabel('Current (mA)')
%title(strcat('Estimated Resistance:', num2str(R_est), ' \Omega'))
%grid on
%hold on
%plot(x, y)
%legend('Automated Current', 'Trendline')
%
%%%
%
%awg_voltage = 1:10;
%current_measured = [0.47117,0.94222,1.476,1.9681,2.4593,2.9514,3.4436,3.9363,4.4298,4.9235];
%figure
%plot(awg_voltage, current_measured, '*', 'Linewidth', 2)
%trendline = polyfit(awg_voltage,current_measured,1);
%x = linspace(0, 10, 100);
%y = trendline(1)*x + trendline(2);
%
%R_est=(sum(awg_voltage.^2)/sum(awg_voltage.*current_measured))*1000;
%
%hold on
%plot(x, y)
%xlabel('Voltage (V)')
%ylabel('Current (mA)')
%title(strcat('Estimated Resistance:', num2str(R_est), ' \Omega'))
%grid on
%legend('Measured Current', 'Trendline')%
