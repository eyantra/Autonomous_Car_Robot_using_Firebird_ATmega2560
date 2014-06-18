%>=======================================================================
%> @file
%>
%> @author sushil kumar meena (sushilm.iitb@gmail.com)
%> @author palak dalal (palak.dalal@gmail.com)
%> @author mudit malpani (cooldudemd@gmailcom)
%> @author hemant noval (novalhemant@gmail.com)
%> 
%> @brief This function applies flood fill algorithm iteratively using
%> queues. This replaces targetcolor with replacecolor
%>
%> @param cxx x-coordinate of the starting pixel
%> @param cyy y-coordinate of the starting pixel
%> @param targetcolor color to be replaced
%> @param replacementcolor color by which targetcolor is replaced
%>=======================================================================


function FloodFill(cxx,cyy,targetcolor, replacementcolor)
global A8
global r;
A8;

enqueue=1;dequeue=1;
[r,c] = size(A8);
queue = zeros(r*c,2);
if A8(cxx,cyy) == targetcolor
    A8(cxx,cyy) = replacementcolor;
    queue(enqueue,1)=cxx;
    queue(enqueue,2)=cyy;
    enqueue = enqueue+1;
    while enqueue~=dequeue %while queue not empty
        nx=queue(dequeue,1);
        ny=queue(dequeue,2);
        dequeue=dequeue+1;
        if A8(nx,ny)==targetcolor
            A8(nx,ny)=replacementcolor;
        end
        
        %>checking west
        if A8(nx-1,ny)==targetcolor
            A8(nx-1,ny)=replacementcolor;
            queue(enqueue,1)=nx-1;
            queue(enqueue,2)=ny;
            enqueue = enqueue+1;
        end
        %>checking east
        if A8(nx+1,ny)==targetcolor
            A8(nx+1,ny)=replacementcolor;
            queue(enqueue,1)=nx+1;
            queue(enqueue,2)=ny;
            enqueue = enqueue+1;
        end
        %>checking north
        if A8(nx,ny-1)==targetcolor
            A8(nx,ny-1)=replacementcolor;
            queue(enqueue,1)=nx;
            queue(enqueue,2)=ny-1;
            enqueue = enqueue+1;
        end
        %>checking south
        if A8(nx,ny+1)==targetcolor
            A8(nx,ny+1)=replacementcolor;
            queue(enqueue,1)=nx;
            queue(enqueue,2)=ny+1;
            enqueue = enqueue+1;
        end
    end
    
end