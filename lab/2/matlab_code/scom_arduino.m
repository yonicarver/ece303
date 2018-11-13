clear ; close all; clc

arduino = serial('COM6', 'Baudrate', 9600);
fopen(arduino);
% arduino.ReadAsyncMode='manual';

buffer = strings(1,2);

while ~0
% for i=1:inf
    counter = 0;
    flushinput(arduino);
    
% 	fprintf(s,'%d',data_out);

    payload = string(fgetl(arduino));
    fwrite(arduino, 'startt');
%     counter = counter + 1;
%     buffer(counter) = payload;
%     if (contains(payload, 'goodbyeworld') || contains(payload,'goodbyeworldgoodbyeworld'))
%         buffer(2) = buffer(1);
%         buffer(1) = nan;
%         disp(buffer)
% %             payload = "estop_status: test_estop, main_relay: test_relay, load_cell: 0, optical_counter: 0, temp: test_temp, coolant: test_coolant, led_normal_op: test_led_normalop, led_high_temp: test_led_hightemp, led_low_coolant: test_led_lowcoolant";
%     else
%             buffer(2) = buffer(1);
%             buffer(1) = payload;
%             disp(buffer);
%         
%     end
    
%     if (contains(payload,'goodbyeworldgoodbyeworld') || contains(payload, 'goodbyeworld'))
%         payload = buffer(counter);
%         counter = counter + 1;
%     end
%     buffer(1) = payload;
    
%     payload = str2double(fgetl(arduino));

%     payload = buffer(2);
%     disp(buffer(counter))
	disp(payload)
%     disp('end of payload');

%     fprintf('\n');
%     fprintf('\n');
    
end
% response = str2double(fgetl(arduino));

%%

fclose(arduino);
delete(arduino);
instrreset
