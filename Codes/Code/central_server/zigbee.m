fid = fopen('pathD');
C =  textscan(fid,'%d');
data = C{1}
[n m] = size(data)
i=1;
    ser = serial('COM43');
    fopen(ser);

while i<= n
    
    dir = data(i);
    i=i+1;
    lanes_x = data(i);
    i=i+1;
    lanes_y = data(i);
    i=i+1;

    dir_b = dec2bin(dir,2);
    lanes_xb = dec2bin(lanes_x,3);
    lanes_yb = dec2bin(lanes_y,3);
    f=strcat(dir_b,lanes_xb,lanes_yb)
    no = 50 * dir + 6 * lanes_x + lanes_y+0
        
    fprintf(ser,'%c',char(no));
    
    pause(3);                                                                                                                                                                                                                                                       
end






























































fprintf(ser,'%c','b');
fclose(ser);