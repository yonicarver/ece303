%fclose(instrfindall);delete(instrfindall)
close all;clear all;clc

% Inputs
wfg_address=10;
scope_address=7;
pulse_frequency=1400; % Pulse frequency (Hz)
pulse_amplitude=5; % Pulse amplitude (V)
dutycycle = [20:5:80,75:-5:20];
volts_per_division=2; % Scope Volts/division
time_range='3E-3'; % Scope time range (s)

% Initiate equipment connection and create device objects
wfg=gpib('NI',0,wfg_address);
scope=gpib('NI',0,scope_address);

% Define scope input and output buffer sizes
scope.InputBufferSize = 2^17;
scope.OutputBufferSize=2^17;

% Open instruments
fopen(wfg);
fopen(scope);

% Set equipment up
fprintf(wfg,'OUTP:LOAD INF'); % High Z
offset=pulse_amplitude/2;
freq_str=num2str(pulse_frequency);
amp_str=num2str(pulse_amplitude);
offset_str=num2str(offset);
cmd_str=['APPL:SQU ' freq_str ',' amp_str ',' offset_str];
fprintf(wfg,cmd_str); % Outputs a square wave
range_str=[':CHANNEL1:RANGE ' num2str(8*volts_per_division) '\n'];
fprintf(scope,range_str); % 16 V (8 divisions, 2V/div)
% Set the time range of the scope (s)
fprintf(scope,[':TIMEBASE:MODE NORMAL; RANGE ' time_range]);
fprintf(scope,':TIMEBASE:DELAY 0US'); % Sets time delay
f = zeros(size(dutycycle));
data=zeros(2000,length(dutycycle));

tv=query(scope,':CHANNEL1:RANGE?;'); % Gets the vertical range of scope (V)
offset=query(scope,':CHANNEL1:OFFSET?'); % Gets the vertical offset (V)
dat=zeros(2000,length(dutycycle));


figure
for I = 1:length(dutycycle)
	fprintf(wfg,'PULS:DCYC %i', dutycycle(I));
	fprintf(scope,'WAVE:DATA?; *OPC');
	dat(:,I) = binblockread(scope);
	f(I)= str2double (query(scope,':MEAS:FREQUENCY?'));
end

% Convert dat from binary representation to actual voltage
tv_num=str2double(tv);
offset_num=str2double(offset);
dat_volts=-tv_num/2+dat/256*tv_num+offset_num;

% Close and delete the AWG and scope objects
fclose(wfg);
fclose(scope);
delete(wfg)
delete(scope)