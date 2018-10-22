%% Draining a Battery-USB
close all;clear all;clc

% Inputs
volts_per_division=2;
time_range='3E-3';

voltage_reading = zeros(1, 4000);
current_reading = zeros(1, 4000);

% Create equipment connections and objects
DMM=visa('agilent','USB0::0x2A8D::0xB318::MY58170025::0::INSTR');
scope=visa('agilent','USB0::0x0957::0x1799::MY58100813::0::INSTR');

scope.InputBufferSize=2^17;
scope.OutputBufferSize=2^17;

% Open instruments
fopen(DMM);
fopen(scope);

range_str=[':CHANNEL1:RANGE ' num2str(8*volts_per_division) '\n'];
fprintf(scope,range_str);
fprintf(scope,[':TIMEBASE:MODE NORMAL; RANGE ' time_range]);
fprintf(scope,[':AUTOSCALE;']);

fprintf(scope,'WAV:SOUR CHAN1;');
fprintf(scope,'WAV:FORM WORD;');
fprintf(scope,'WAV:POINTS:MODE RAW');
fprintf(scope,'WAV:POIN 2000');
fprintf(scope,':WAVEFORM:BYTEORDER LSBFirst');

tv=query(scope,':CHANNEL1:RANGE?;');
offset=query(scope,':CHANNEL1:OFFSET?');

i = 1;
tic
while (i)
	pause(i-toc)

    disp(i);

   	curr = str2double(query(DMM,'MEAS:CURR:DC?')); % Measure Current
    current_reading(i) = curr;
    disp(current_reading(i));

    voltage_reading(i) = str2double(query(scope, ':MEAS:VAV? CHAN1'));
    disp(voltage_reading(i));
    

    if (voltage_reading(i) < 4.8)
        disp('done');
        break;
    end
    
	i = i+1;
    
end

%%
% Close and delete instrument connections and objects
fclose(DMM);
fclose(scope);

delete(DMM);
delete(scope);

%%
% Remove zero values at the end of the vectors
while (voltage_reading(end) == 0 && current_reading(end) == 0)
    voltage_reading(end) = [];
    current_reading(end) = [];
end

%%
% Calculate Time, Power, and Energy
time = 1:length(voltage_reading);
instantaneous_power = voltage_reading .* current_reading;
cumulative_energy = cumsum(instantaneous_power);

%%
% Plot graphs
figure(1)
plot(time, voltage_reading)
ylabel('Battery Voltage (V)')
xlabel('Time (s)')
title('Battery Voltage vs. Time')
grid on

figure(2)
plot(time, current_reading)
ylabel('Battery Current (I)')
xlabel('Time (s)')
title('Battery Current vs. Time')
grid on

figure(3)
plot(time, instantaneous_power)
ylabel('Instantaneous Power (W)')
xlabel('Time (s)')
title('Battery Instantaneous Power vs. Time')
grid on

figure(4)
plot(time, cumulative_energy)
ylabel('Cumulative Energy (J)')
xlabel('Time (s)')
title('Battery Cumulative Energy vs. Time')
grid on
