% Resistor Estimation Lab
close all;clear all;clc

% Inputs
wfg_address=10; % Waveform generator address
mm_address=22; % Multimeter address
V_min=0; % Lower DC voltage (Volts)
V_max=9; % Upper DC voltage (Volts)
N_volts=11; % Number of voltages to cycle through

% First, initiate communications setup with measurment equipment
wfg=gpib('NI',0,wfg_address);
mm=gpib('NI',0,mm_address);
% Open instruments
fopen(wfg);
fopen(mm);

% Place waveform generator into High Z termination
fprintf(wfg,'OUTP:LOAD INF'); % High Z
% Conduct experiment by sweeping waveform generator voltage
V=linspace(V_min,V_max,N_volts);
Imeas=zeros(1,N_volts);
for K=1:N_volts
	str1=['APPL:DC DEF,DEF,' num2str(V(K))];
	fprintf(wfg,str1);
	pause(1) % Wait for everything to settle out
	current=str2double(query(mm,'MEAS:CURR:DC? 1e-3,1e-6'));
	Imeas(K)=current;
end

% Close and delete instrument connections and objects
fclose(mm);
fclose(wfg);
delete(mm)
delete(wfg)