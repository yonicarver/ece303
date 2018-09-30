function [t,volts]=read_scope(obj,channel,num_points)

% This function reads the scope
fprintf(obj,'WAV:FORM WORD;');
fprintf(obj,'WAVEFORM:BYTEORDER LSBFirst');
fprintf(obj,['WAV:SOUR CHAN' num2str(channel)]);
% Set timebase to main
fprintf(obj,':TIMEBASE:MODE MAIN');
% Set up acquisition type and count.
fprintf(obj,':ACQUIRE:TYPE NORMAL');
fprintf(obj,':ACQUIRE:COUNT 1');
fprintf(obj,[':WAV:POIN ' num2str(num_points)]);
fprintf(obj,':WAV:POINTS:MODE RAW');
fprintf(obj,['DIG CHAN' num2str(channel) ';']);
operationComplete=str2double(query(obj,'*OPC?'));
while ˜operationComplete
	operationComplete=str2double(query(obj,'*OPC?'));
end
preambleBlock=query(obj,':WAVEFORM:PREAMBLE?');
fprintf(obj,'WAV:DATA?;');
data=binblockread(obj,'uint16');
fread(obj,1);

% Convert data from binary representation to actual voltage
preamble=str2double(strsplit(preambleBlock,','));
dt=preamble(5);
t0=preamble(6);
t_ref=preamble(7);
dy=preamble(8);
y0=preamble(9);
y_ref=preamble(10);

t=(dt*(1:num_points)-dt).';
volts=(dy.*(data-y_ref))+y0;
end
