img = imread('TEST_4.jpeg');

%togglefig('Red Plane, Adjusted')
red = img(:,:,2);
red = adapthisteq(red);
[accum, circen, cirrad] = CircularHough_Grd(red,[5,100]);
circen
cirrad
img(189,24,1)=0;
img(189,24,2)=0;
img(189,24,3)=0;
[r c]=size(red);


%imshow(img);
