clear ; close all; clc

arduino = serial('COM6', 'Baudrate', 9600);
fopen(arduino);
% arduino.ReadAsyncMode='manual';
%%
buffer = strings(1,2);

while ~0
% for i=1:inf
    counter = 0;
    flushinput(arduino);
    
% 	fprintf(s,'%d',data_out);

%     fwrite(arduino, 'start');
    
    start_stop_i = 1;
    max_rpm_i = 22;
    max_load_i = 1;
    
    fwrite(arduino, '%01i,%04i,%04i*', [start_stop_i, max_rpm_i, max_load_i]);
%     fprintf(arduino, '%01i,%04i,%04i*', [start_stop_i, max_rpm_i, max_load_i]);
%     fscanf(arduino);
    payload = string(fgetl(arduino));

	disp(payload)

end
% response = str2double(fgetl(arduino));

%%

fclose(arduino);
delete(arduino);
instrreset
