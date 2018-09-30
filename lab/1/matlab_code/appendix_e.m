%% Lab 2B-Automation With AWG-GPIB
%This simulates two scope inputs and captures the data from each channel
%for various duty cycles
close all;clear all;clc
% Inputs
awg_address=10;
scope_address=7;
pulse_frequency=1000; % Pulse frequency (Hz)
pulse_amplitude=5; % Pulse amplitude (V)
duty_cycle=20:5:80; % Duty cycle (%)
volts_per_division_ch1=2; % Scope Volts/division for scope Channel 1
volts_per_division_ch2=2; % Scope Volts/division for scope Channel 2

% Initiate equipment connection and create device objects
awg=gpib('NI',0,awg_address);
scope=gpib('NI',0,scope_address);
scope.InputBufferSize = 2ˆ17;
scope.OutputBufferSize = 2ˆ17;

% Open communications
fopen(awg);
fopen(scope);

% Set up the AWG
fprintf(awg,'OUTP:LOAD INF'); % Places the AWG in High Z termination
offset=pulse_amplitude/2; % Pulse offset
freq_str=num2str(pulse_frequency);
amp_str=num2str(pulse_amplitude);
offset_str=num2str(offset);

% Set up the Scope
range_ch1=8*volts_per_division_ch1;
range_ch2=8*volts_per_division_ch2;
range_str_ch1=[':CHANNEL1:RANGE ' num2str(range_ch1) ';'];
range_str_ch2=[':CHANNEL2:RANGE ' num2str(range_ch2) ';'];
fprintf(scope,range_str_ch1); % Sets voltage range for Channel 1
fprintf(scope,range_str_ch2); % Sets voltage range for Channel 2
fprintf(scope,':AUTOSCALE'); % Stops the scope data acquisition
fprintf(scope,':TIMEBASE:RANGE 5e-3');
fprintf(scope,':TRIGGER:SOURCE CHANNEL1;');
fprintf(scope,':TRIGGER:MODE AUTO;');
fprintf(scope,':TRIGGER:LEVEL 0.2;');

% Get the voltage offset for each channel
offset_ch1=str2double(query(scope,':CHANNEL1:OFFSET?'));
offset_ch2=str2double(query(scope,':CHANNEL2:OFFSET?'));
range_ch1=str2double(query(scope,':CHANNEL1:RANGE?'));
range_ch2=str2double(query(scope,':CHANNEL2:RANGE?'));

% Create data and frequency vectors for each channel
data_ch1=zeros(1000,length(duty_cycle));
data_ch2=zeros(1000,length(duty_cycle));
freq_ch1=zeros(1,length(duty_cycle));
freq_ch2=zeros(1,length(duty_cycle));
%Conduct the sweep
for I=1:length(duty_cycle)
	fprintf(awg,'PULS:DCYC %i',duty_cycle(I)); % Sets the duty cycle (%)
	pause(1) % This is done to allow the electronics to settle
	fprintf(scope,':STOP'); % Stops the scope data acquisition
	fprintf(scope,':WAVEFORM:SOURCE CHANNEL1;');
	fprintf(scope,':WAVE:DATA?');
	data_ch1(:,I)=binblockread(scope);
	fprintf(scope,':WAVEFORM:SOURCE CHANNEL2;');
	fprintf(scope,':WAVE:DATA?');
	data_ch2(:,I)=binblockread(scope);
	fprintf(scope,':MEAS:SOURCE CHANNEL1;');
	freq_ch1(I)=str2double(query(scope,':MEAS:FREQUENCY?'));
	fprintf(scope,':MEAS:SOURCE CHANNEL2;');
	freq_ch2(I)=str2double(query(scope,':MEAS:FREQUENCY?'));
	fprintf(scope,':RUN'); % Continues the scope data acquisition
end

% Convert data from binary representation to actual voltage
volts_ch1=-range_ch1/2+data_ch1/256*range_ch1+offset_ch1;
volts_ch2=-range_ch2/2+data_ch2/256*range_ch2+offset_ch2;

% Close and delete the AWG and scope objects
fclose(awg);
fclose(scope);
delete(awg)
delete(scope)

% Plot results
figure
plot(volts_ch1)
figure
plot(volts_ch2)