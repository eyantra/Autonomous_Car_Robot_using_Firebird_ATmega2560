%>@file
%>
%>---------------original ocr authors--------------------
%> OCR (Optical Character Recognition).
%> Author: Ing. Diego Barragán Guerrero
%> e-mail: diego@matpic.com
%> For more information, visit: www.matpic.com
%>________________________________________
%> PRINCIPAL PROGRAM
%>
%>========================================================
%> @author sushil kumar meena (sushilm.iitb@gmail.com)
%> @author palak dalal (palak.dalal@gmail.com)
%> @author mudit malpani (cooldudemd@gmailcom)
%> @author hemant noval (novalhemant@gmail.com)

%>This matlab code is capable of capturing images and %
%>detecting red and green light of some particular area
%>(configurable) and recognizing characters characters
%>For the purpose of practicality, Characters are placed
%>inside a blue circle. Depending on the red, green or character
%>recognized, it then does do zegbee communication.
%>
%>zegbee communication code
%>a--->red light
%>b--->green light
%>c--->number 30
%>d--->number 60
%>e--->number 90



%>initializing zegbee communication port
ser = serial('COM43');
fopen(ser);
sec = .01;
while(1)
     i=0;
     pause(.01);
     orgImage = getsnapshot(vid);
     flushdata(vid);
    %figure, imshow(orgImage);
%    orgImage=imread('speednew.jpg');
    [code]=trafficLight(orgImage);
    if(code==1) %>red light found
        code
        fprintf(ser,'%c','a');
        pause(3);
        continue;
    end
    if(code==2) %>green light found
        code
        fprintf(ser,'%c','b');
        continue;
    end
    [imagen]=extractingCharImg(orgImage);
    
    %figure,imshow(imagen);
    %> Remove all object containing fewer than 30 pixels
    imagen = bwareaopen(imagen,30);
    %>Storage matrix word from image
    word=[ ];
    word_read=[];
    re=imagen;
    
    %> Load templates
    load templates
    global templates
    %> Compute the number of letters in template file
    num_letras=size(templates,2);
    while 1
        %>Fcn 'lines' separate lines in text
        [row cols]=size(re);
        check=0;
        %>re with all black values give some error%
        %>this loop is to deal with that error %
        for i=1:row
            for j=1:cols
                if(re(i,j)==1)
                    check=1;
                    break;
                end
            end
        end
        if check==0
            break;
        end
        [fl re]=lines(re);
        imgn=fl;
        
        %>Uncomment line below to see lines one by one
        %>imshow(fl);pause(0.5)
        %>-----------------------------------------------------------------
        %> Label and count connected components
        [L Ne] = bwlabel(imgn);
        for n=1:Ne
            [r,c] = find(L==n);
            %> Extract letter
            n1=imgn(min(r):max(r),min(c):max(c));
            %> Resize letter (same size of template)
            img_r=imresize(n1,[42 24]);
            %>Uncomment line below to see letters one by one
            %imshow(img_r);pause(0.5)
            %-------------------------------------------------------------------
            %> Call fcn to convert image to text
            letter=read_letter(img_r,num_letras);
            %
            %>Letter concatenation
            word=[word letter];
        end
        %fprintf(fid,'%s\n',lower(word));%Write 'word' in text file (lower)
        %fprintf(fid,'%s\n',word);%Write 'word' in text file (upper)
        % Clear 'word' variable
        word_read=word;
        word=[ ];
        %>When the sentences finish, breaks the loop
        if isempty(re)  %See variable 're' in Fcn 'lines'
            break
        end
    end
    
    %>zigbee communication depending on words read or signals captured
    [num] = size(word_read);
    if(num==0)
        fprintf(ser,'%c','b');
        20   %>blue circle detected but no character inside
        continue;
    end
    if(num==1)
        fprintf(ser,'%c','b');
        20   %>blue circle detected but no character inside
        continue;
    end
    if(word_read(1)=='3' && word_read(2)=='O') %>speed limit 30 found
       30
       fprintf(ser,'%c','c');
       continue;
    end
    if(word_read(1)=='6' && word_read(2)=='O') %>speed limit 60 found
        60
        fprintf(ser,'%c','d');
        continue;
    end
    if(word_read(1)=='9' && word_read(2)=='O')%>speed limit 90 found
        90
        fprintf(ser,'%c','e');
        continue;
    end
    fprintf(ser,'%c','b');
    25   %>nothing special detected
    
    
    
end
fclose(ser);
%stop(vid);
fprintf('For more information, visit: <a href= "http://www.matpic.com">www.matpic.com </a> \n')
clear all