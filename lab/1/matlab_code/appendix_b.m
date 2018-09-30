%% Resistor Estimation Lab-USB
close all;clear all;clc

% Inputs
V_min=0; % Lower DC Voltage (V)
V_max=10; % Upper DC voltage (V);
N_volts=101; % Number of voltages to cycle through

% Create equipment connections and objects
DMM=visa('agilent','USB0::0x2A8D::0xB318::MY58170025::0::INSTR');
awg=visa('agilent','USB0::0x0957::0x0407::MY44043469::0::INSTR');

% Open instruments
fopen(awg);
fopen(DMM);

fprintf(awg,'OUTP:LOAD INF'); % Place waveform generator into high-Z

% Conduct experiment by sweeping waveform generator voltage
V=linspace(V_min,V_max,N_volts);
Imeas=zeros(1,N_volts);
for K=1:N_volts
	str1=['APPL:DC DEF,DEF,' num2str(V(K))];
	fprintf(awg,str1); % Apply DC Voltage Output from AWG
	pause(1)
	current=str2double(query(DMM,'MEAS:CURR:DC?')); % Measure Current
	Imeas(K)=current;
end

% Close and delete instrument connections and objects
fclose(DMM);
fclose(awg);
delete(DMM)
delete(awg)

% Estimate Resistor Value
R_est=sum(V.^2)/sum(V.*Imeas);

%%

trendline = polyfit(V,Imeas*1000,1);
x = linspace(0, 10, 100);
y = trendline(1)*x + trendline(2);

figure
plot(V,Imeas*1000,'o','MarkerFaceColor','b')
xlabel('Voltage (V)')
ylabel('Current (mA)')
title(strcat('Estimated Resistance:', num2str(R_est), ' \Omega'))
grid on
hold on
plot(x, y)
legend('Automated Current', 'Trendline')

%%

awg_voltage = 1:10;
current_measured = [0.47117,0.94222,1.476,1.9681,2.4593,2.9514,3.4436,3.9363,4.4298,4.9235];
figure
plot(awg_voltage, current_measured, '*', 'Linewidth', 2)
trendline = polyfit(awg_voltage,current_measured,1);
x = linspace(0, 10, 100);
y = trendline(1)*x + trendline(2);

R_est=(sum(awg_voltage.^2)/sum(awg_voltage.*current_measured))*1000;

hold on
plot(x, y)
xlabel('Voltage (V)')
ylabel('Current (mA)')
title(strcat('Estimated Resistance:', num2str(R_est), ' \Omega'))
grid on
legend('Measured Current', 'Trendline')