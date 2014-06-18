%-----------------------ExtractingCharImg---------------------------%
%authors: sushil, mudit, palak, hemant
%This function takes an image (rgb). It detects a blue ring and return
%its inside content. The image returned is a binary image that has
%only dark objects that are inside the blue ring, in white color
%over a black background .Some noise tolerance is done here. Color
%value for blue is configurable and depends on the lightning condition
%---------------------------------------------------------------------%

function [ finalImg ] = extractingCharImg( orgImage )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%--------------------------configVariables----------------------------%
redUpper=120;
greenUpper=120;
blueLower=150;
binaryForRedCircleThres=0.2;
structElemCircleSmooth = strel('square', 6);
structElemCharSmooth = strel('square', 4);
%---------------------------------------------------------------------%


redRemovedImg = orgImage;
[r c d] = size(orgImage);
for i=1:r
    for j=1:c
        if(orgImage(i,j,1) < redUpper && orgImage(i,j,2) <greenUpper && orgImage(i,j,3) >blueLower)
            redRemovedImg(i,j,1)=255;
            redRemovedImg(i,j,2)=255;
            redRemovedImg(i,j,3)=255;
        end
    end
end
%%%extracting only circle from the blue removed image and smoothening%%%
greyImg = rgb2gray(orgImage);
redGrey = rgb2gray(redRemovedImg);
onlyCircleImg = redGrey - greyImg;
onlyCircleImg= imdilate(onlyCircleImg, structElemCircleSmooth);
onlyCircleImg = imerode(onlyCircleImg, structElemCircleSmooth);

onlyCircleImg = imerode(onlyCircleImg, structElemCircleSmooth);
onlyCircleImg= imdilate(onlyCircleImg, structElemCircleSmooth);
binaryOnlyCircle = im2bw(onlyCircleImg,binaryForRedCircleThres); %%this has the binary image of only circle
%%%left pass..whitening the black components to the left of circle
for i=1:r
    leftStarted=0;
    for j=1:c
        if(binaryOnlyCircle(i,j)==1)
            break;
        else
            binaryOnlyCircle(i,j)=1;
        end
    end
end

%%%right pass... whitening the white components to the right of the circle
for i=1:r
    leftStarted=0;
    for j=1:c
        if(binaryOnlyCircle(i,c-j+1)==1)
            break;
        else
            binaryOnlyCircle(i,c-j+1)=1;
        end
    end
end

%%%whiteing the left the pixels in redGrey which corresponds to white
%%%pixels in binaryOnlyCircle
for i=1:r
    leftStarted=0;
    for j=1:c
        if(binaryOnlyCircle(i,j)==1)
            redGrey(i,j)=255;
        end
    end
end

binaryRedGrey = im2bw(redGrey);
%%%smoothening the figure
finalImg = imerode(binaryRedGrey, structElemCharSmooth);
finalImg = imdilate(finalImg, structElemCharSmooth);
%final image contains the black character over white background in
%inverting binary image
finalImg =~ finalImg;
end

