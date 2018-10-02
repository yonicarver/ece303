clear ; close all; clc

arduino = serial('COM3', 'Baudrate', 9600);
fopen(arduino);

while ~0
    fwrite(arduino, '1');
    flushinput(arduino);

    resp = str2double(fgetl(arduino));
    disp(resp)
end
% response = str2double(fgetl(arduino));



fclose(arduino);
delete(arduino);