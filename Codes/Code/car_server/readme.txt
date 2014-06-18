%%%%%%%%%%%%%%%%%%%%%%documentation%%%%%%%%%%%%%%%%%%%%%%
@ authors: 
Sushil Kumar Meena,
Palak Dalal,
Mudit Malpani
Hemant Noval

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

This file contains a brief introduction of the files present in this folder. Actual details can be found in the respective matlab files.
Files present:
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
OCR.M:
This is the main file to run. It useses several other functions. It first test for presence red circle, then green circle and then a blue ring which contains a text. That text is extracted using ocr. 
Depending on what is detected and the value of characters given by ocr, zigbee communication is accomplished.

*****************************functions************************************
1.extractingCharImg.m:

This function takes a rgb image and returns a binary image that contains the character.

2.TrafficLight.m:

This function takes a red image and return codes that determine whether red blob or green blob of a particular size was detected or nothing was detected.

3. Other files: There are several other function files that are used as auxilliary function by OCR and their deep understanding is not important for running ocr.