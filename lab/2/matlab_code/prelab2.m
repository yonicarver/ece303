%% ECE 303, Lab 2
%% Yonatan Carver & Farhan Muhammad
%% 10.1.2018


close all; clear all; clc

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

% Arduino Serial Communication
arduino=serial('COM28', 'Baudrate', 9600);
fopen(arduino);

fprintf(awg,'OUTP:LOAD INF'); % Place waveform generator into high-Z
 
% Conduct experiment by sweeping waveform generator voltage
V=linspace(V_min,V_max,N_volts);
fscope=zeros(1,N_volts);
arduino_count = zeros(1, N_volts);

for K=1:N_volts
	str1=['APPL:DC DEF,DEF,' num2str(V(K))];
	fprintf(awg,str1); % Apply DC Voltage Output from AWG
     for i=1:4
         pause(1)
         fwrite(arduino, 'reset');
     end
    pause(1)
    flushinput(arduino);
    arduino_count(K)=str2double(fgetl(arduino));
    disp(arduino_count(K))
    fwrite(arduino, 'reset');
    fscope(K)=str2double(query(scope,':MEAS:FREQUENCY?'));
% 	dcyc=str2double(query(scope,'PULS:DCYC?')); % Measure duty cycle
% 	duty_cycle(K)=dcyc;
    str1=['APPL:DC DEF,DEF,' num2str(0)];
	fprintf(awg,str1); % Apply DC Voltage Output from AWG
end

% Close and delete instrument connections and objects
% fclose(DMM);
fclose(awg);
fclose(scope);
fclose(arduino);
% delete(DMM)
delete(awg)
delete(scope)
delete(arduino)

%%

m = V\arduino_count;
x = linspace(0, 5, 51);
y = m(51,51)*x;
 
figure(1)
plot(V,arduino_count,'o','MarkerFaceColor','b')
ylabel('RPM')
xlabel('Voltage (V)')
title('RPM vs. Voltage (Arduino Pulse Count)')
hold on
plot(x,y)
legend('Measured Data', 'Best Fit')
grid on

m2 = V\arduino_count;
x2 = linspace(0, 5, 51);
y2 = m(51,51)*x2;
 
figure(2)
plot(V,fscope,'o','MarkerFaceColor','b')
ylabel('RPM')
xlabel('Voltage (V)')
title('RPM vs. Voltage (Optical Counter)')
hold on
plot(x2,y2)
legend('Measured Data', 'Best Fit')
grid on

figure(3)
plot(V,arduino_count,'o','MarkerFaceColor','b')
ylabel('RPM')
xlabel('Voltage (V)')
title('RPM vs. Voltage (Arduino Pulse Count)')
hold on
plot(V,fscope,'o','MarkerFaceColor','r')
legend('Data (arduino)', 'Data(scope)')
grid on