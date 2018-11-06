%% ECE 303, Lab 2
%% Yonatan Carver & Farhan Muhammad
%% 10.15.2018
close all; clear all; clc

% estop_status
% main_relay
% load_cell
% optical_counter
% temp
% coolant
% led_normal_op
% led_high_temp
% led_low_coolant


% Arduino Serial Communication
arduino=serial('COM6', 'Baudrate', 9600);
fopen(arduino);

% rpm = zeros(1, 10000);

for i=1:inf
    flushinput(arduino);

    payload = fgetl(arduino);
%     disp(payload);
    split = strsplit(payload, {', ', ': '});
%     disp(split)
    
    estop_status = split([find(strcmp(split, 'estop_status'))]+1);
    main_relay = split([find(strcmp(split, 'main_relay'))]+1);
    load_cell = split([find(strcmp(split, 'load_cell'))]+1);
    optical_counter = split([find(strcmp(split, 'optical_counter'))]+1);
    temp = split([find(strcmp(split, 'temp'))]+1);
    coolant = split([find(strcmp(split, 'coolant'))]+1);
    led_normal_op = split([find(strcmp(split, 'led_normal_op'))]+1);
    led_high_temp = split([find(strcmp(split, 'led_high_temp'))]+1);
    led_low_coolant = split([find(strcmp(split, 'led_low_coolant'))]+1);
    
    disp(estop_status)
    disp(main_relay)
    disp(load_cell)
    disp(optical_counter)
    disp(temp)
    disp(coolant)
    disp(led_normal_op)
    disp(led_high_temp)
    disp(led_low_coolant)
    
    fprintf('\n')
    
    
end

%% Close and delete instrument connections and objects
fclose(arduino);
delete(arduino);
%%
instrreset
