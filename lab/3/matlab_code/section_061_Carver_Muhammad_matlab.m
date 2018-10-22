%% ECE 303, Lab 2
%% Yonatan Carver & Farhan Muhammad
%% 10.15.2018

close all; clear all; clc

% Inputs

max_rpm = 225/2;

% Arduino Serial Communication
arduino=serial('COM3', 'Baudrate', 9600);
fopen(arduino);

rpm = zeros(1, 10000);


for i=1:inf
    flushinput(arduino);
    rpm(i) = (str2double(fgetl(arduino)));
    disp(rpm(i))

	if rpm(i) > max_rpm/2
		fwrite(arduino,'fan_on');
	elseif rpm(i) < max_rpm/2
		fwrite(arduino, 'fan_off');
	end

end

%% Close and delete instrument connections and objects
fclose(arduino);
delete(arduino)
instrreset
