clear ; close all; clc

arduino = serial('COM6', 'Baudrate', 9600);
fopen(arduino);

% while ~0
for i=1:inf

    flushinput(arduino);

    payload = fgetl(arduino);
    disp(payload)
end
% response = str2double(fgetl(arduino));

%%

fclose(arduino);
delete(arduino);
instrreset
