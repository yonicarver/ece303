%% prelab, lab 2
close all;clear all;clc

% Inputs
V_min=0; % Lower DC Voltage (V)
V_max=5; % Upper DC voltage (V);
N_volts=51; % Number of voltages to cycle through

% Create equipment connections and objects
% DMM=visa('agilent','USB0::0x2A8D::0xB318::MY58170025::0::INSTR');
awg=visa('agilent','USB0::0x0957::0x0407::MY44043469::0::INSTR');
scope=visa('agilent','USB0::0x0957::0x1799::MY58100813::0::INSTR');

% Open instruments
fopen(awg);
fopen(scope);
% fopen(DMM);

fprintf(awg,'OUTP:LOAD INF'); % Place waveform generator into high-Z

% Conduct experiment by sweeping waveform generator voltage
V=linspace(V_min,V_max,N_volts);
% Imeas=zeros(1,N_volts);
duty_cycle = zeros(1, N_volts);

for K=1:N_volts
	str1=['APPL:DC DEF,DEF,' num2str(V(K))];
	fprintf(awg,str1); % Apply DC Voltage Output from AWG
	pause(1)
	dcyc=str2double(query(scope,'PULS:DCYC?')); % Measure duty cycle
	duty_cycle(K)=dcyc;
end

% Close and delete instrument connections and objects
% fclose(DMM);
fclose(awg);
fclose(scope):
% delete(DMM)
delete(awg)
delete(scope)

%%

% trendline = polyfit(V,duty_cycle,1);
% x = linspace(0, 10, 100);
% y = trendline(1)*x + trendline(2);
% 
figure
plot(V,duty_cycle,'o','MarkerFaceColor','b')
xlabel('Voltage (V)')
ylabel('Duty Cycle (%)')
title('Voltage vs. Duty Cycle')
grid on
