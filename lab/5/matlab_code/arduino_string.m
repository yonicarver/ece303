function data = arduino_string()

arduino=serial('COM3', 'Baudrate', 9600);
fopen(arduino);

% rpm = zeros(1, 10000);

for i=1:inf
    flushinput(arduino);

    payload = fgetl(arduino);
%     return payload;
    disp(payload);
    
end

 Close and delete instrument connections and objects
fclose(arduino);
delete(arduino);
instrreset

end