%% ECE 303, Lab 2
%% Yonatan Carver & Farhan Muhammad
%% 10.15.2018

close all; clear all; clc

% Arduino Serial Communication
arduino=serial('COM3', 'Baudrate', 9600);
fopen(arduino);

rpm = zeros(1, 10000);

for i=1:inf
    flushinput(arduino);

    payload = fgetl(arduino);
    disp(payload);
    
end

%% Close and delete instrument connections and objects
fclose(arduino);
delete(arduino);
instrreset
