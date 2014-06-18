%-------------------------------------------------------------%
%authors: sushil, mudit, palak, hemant
%This code takes an Image (rgb). It detects green and red blobs
%in it. Only those blobs whose size are greater than the configured
%size(area). Red and Green values are configurable and depend
%on lighting conditions. It returns 1 if red found, 2 if green
%found, 0 if nothing found.
%--------------------------------------------------------------%


function [ colorCode ] = trafficLight( orgImage)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%------------------------config parameters---------------------%
redUpperForGreen=80;
greenLowerForGreen=100;
blueUpperForGreen=80;

redLowerForRed=150;
greenUpperForRed=100;
blueUpperForRed=100;
structElemWhiteSmooth = strel('square', 16);

minTrafficLightArea = 10000;
%-----------------------config Parameters end------------------%

%%%reading and converting its grayScale to 0 and 255 values according%%%
%%%to what Range of Red values are allowed%%%
colorCode=0;
%figure,imshow(orgImage);
orgGrayImg = rgb2gray(orgImage);
[r c d] = size(orgImage);
for i=1:r
    for j=1:c
        if(orgImage(i,j,1)>redLowerForRed && orgImage(i,j,2)<greenUpperForRed && orgImage(i,j,3)<blueUpperForRed)
            orgGrayImg(i,j)=255;
        else
            orgGrayImg(i,j)=0;
        end
    end
end
%figure, imshow(orgGrayImg);

%%%removing white noise(original red noise)%%%
orgGrayImg = imerode(orgGrayImg, structElemWhiteSmooth);
orgGrayImg = imdilate(orgGrayImg,structElemWhiteSmooth);
orgGrayImg = imdilate(orgGrayImg, structElemWhiteSmooth);
orgGrayImg = imerode(orgGrayImg,structElemWhiteSmooth);
%figure,imshow(orgGrayImg);
imwrite(orgGrayImg, 'onlyCircle.jpg');

%%%Finding connected components%%%
binaryImage = im2bw(orgGrayImg, 0.5);
[L,num] = bwlabel(binaryImage,4);
graindata = regionprops(L, 'basic');
%selecting blobs of a particularly large area%
for i=1:num
    graindata(i).Area
    if (graindata(i).Area > minTrafficLightArea)
        colorCode=1;
    end
end
    
%%%%%%%detecting green light%%%%%%%

%figure,imshow(orgImage);
for i=1:r
    for j=1:c
        if(orgImage(i,j,1)<redUpperForGreen && orgImage(i,j,2)>greenLowerForGreen && orgImage(i,j,3)<blueUpperForGreen)
            orgGrayImg(i,j)=255;
        else
            orgGrayImg(i,j)=0;
        end
    end
end
%figure,imshow(orgGrayImg);
%figure, imshow(orgGrayImg);

%%%removing white noise(original green noise)%%%
orgGrayImg = imdilate(orgGrayImg, structElemWhiteSmooth);
orgGrayImg = imerode(orgGrayImg,structElemWhiteSmooth);
orgGrayImg = imerode(orgGrayImg, structElemWhiteSmooth);
orgGrayImg = imdilate(orgGrayImg,structElemWhiteSmooth);
%figure,imshow(orgGrayImg);
imwrite(orgGrayImg, 'onlyCircle.jpg');

%%%Finding connected components%%%
binaryImage = im2bw(orgGrayImg, 0.5);
%figure,imshow(binaryImage);
[L,num] = bwlabel(binaryImage,4);
graindata = regionprops(L, 'basic');
for i=1:num
    if (graindata(i).Area > minTrafficLightArea)
        colorCode=2;
    end
end
%RGB_label = label2rgb(L);
end

