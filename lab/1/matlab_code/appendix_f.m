%% Lab 2B-Automation With AWG-USB
%This simulates two scope inputs and captures the data from each channel
%for various duty cycles
close all;clear all;clc

% Inputs
awg_address='USB0::0x0957::0x0407::MY44043483::0::INSTR';
scope_address='USB0::0x0957::0x1799::MY58100818::0::INSTR';
pulse_frequency=1000; % Pulse frequency (Hz)
pulse_amplitude=5; % Pulse amplitude (V)
duty_cycle=20:5:80; % Duty Cycle (%)
duty_cycle=20;
volts_per_division_ch1=0.5; % Scope Volts/division for scope Channel 1
volts_per_division_ch2=2; % Scope Volts/division for scope Channel 2
num_points=1000;

% Create equipment objects
awg=visa('agilent',awg_address);
scope=visa('agilent',scope_address);
scope.InputBufferSize=1e7;
scope.OutputBufferSize=1e7;

% Open communications
fopen(awg);
fopen(scope);

% Set up the AWG
fprintf(awg,'OUTP:LOAD INF'); % Places the AWG in High Z termination
offset=pulse_amplitude/2; % Pulse offset
freq_str=num2str(pulse_frequency);
amp_str=num2str(pulse_amplitude);
offset_str=num2str(offset);
% Set up the scope
fprintf(scope,'*RST; :AUTOSCALE');
fprintf(scope,':TRIGGER:SOURCE CHANNEL1;');
fprintf(scope,':TRIGGER:MODE AUTO;');
fprintf(scope,':TRIGGER:LEVEL 0.1;');

% Create data and frequency vectors for each channel
volts_ch1=zeros(num_points,length(duty_cycle));
times_ch1=zeros(num_points,length(duty_cycle));
volts_ch2=zeros(num_points,length(duty_cycle));
times_ch2=zeros(num_points,length(duty_cycle));
freq_ch1=zeros(1,length(duty_cycle));


freq_ch2=zeros(1,length(duty_cycle));
fprintf(scope,':RUN');

%Conduct the sweep
for I=1:length(duty_cycle)
	fprintf(awg,'PULS:DCYC %i',duty_cycle(I)); % Sets the duty cycle (%)
	pause(1) % This is done to allow the electronics to settle
	fprintf(scope,':STOP'); % Stops the scope data acquisition
	fprintf(scope,':WAVEFORM:SOURCE CHANNEL1;');
	[t1,dat_ch1]=read_scope_usb(scope,1,num_points);
	fprintf(scope,':MEAS:SOURCE CHANNEL1;');
	freq_ch1(I)=str2double(query(scope,':MEAS:FREQUENCY?'));
	[t2,dat_ch2]=read_scope_usb(scope,2,num_points);
	fprintf(scope,':MEAS:SOURCE CHANNEL2;');
	freq_ch2(I)=str2double(query(scope,':MEAS:FREQUENCY?'));
	times_ch1(:,I)=t1;
	times_ch2(:,I)=t2;
	volts_ch1(:,I)=dat_ch1;
	volts_ch2(:,I)=dat_ch2;
	fprintf(scope,':RUN'); % Continues the scope data acquisition
end

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