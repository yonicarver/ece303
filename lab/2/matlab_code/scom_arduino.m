clear ; close all; clc

arduino = serial('COM6', 'Baudrate', 9600);
fopen(arduino);

while ~0
% for i=1:inf

    flushinput(arduino);

%     payload = fgetl(arduino);
    payload = str2double(fgetl(arduino));
    disp(payload)
    fprintf('\n');
end
% response = str2double(fgetl(arduino));

%%

fclose(arduino);
delete(arduino);
instrreset
