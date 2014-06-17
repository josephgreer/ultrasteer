%back proj
figure;
I=imread('needles1\dataset_grid\im_00032.jpeg');
imshow(I);
hold on;
axis on;
I=double(I);
i=0;
while 1>0
    i=i+1
    [x,y]=ginput(1);
    x=round(x);
    y=round(y);
    ss=0;
    ii=0;
    for k=x-1:x+1
        for m=y-1:y+1
            ss=ss+I(m,k)*[m,k];
            ii=ii+I(m,k);
        end
    end
    s(i,:)=round(ss/ii);
    
%    tmp=I(y-1:y+1,x-1:x+1);
%    [v1,m1]=min(tmp);
%    [v2,m2]=min(v1);
%    s(i,:)=[m1(m2),m2];
%    s(i,:)=s(i,:)+[y-1,x-1]-1;
    plot(s(i,2),s(i,1),'*','color','y');
end
