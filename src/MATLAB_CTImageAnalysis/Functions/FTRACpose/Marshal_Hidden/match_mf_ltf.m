function f=match_mf_ltf(cap)
%t0=clock;
%initialize the preflow;
[n_row,n_col]=size(cap);
h=zeros(n_row,1);
h(1)=n_row;
e=zeros(n_row,1);
f=zeros(n_row);

for j=2:n_col
    if cap(1,j)>0
        f(1,j)=cap(1,j);
        f(j,1)=-cap(1,j);
        e(j)=cap(1,j);
    end
end

%initialize the list of overflowing vertices;
L=2:n_row-1;

%initialize the neighboring lists;
N=cell(n_row-2,1);
for k=2:n_row-1
    m=1;
    Ntmp=[];
    for i=1:n_row
        if i~=k & cap(i,k)>0 
            Ntmp(m)=i;
            m=m+1;
        end
    end
    for j=1:n_row
        if j~=k & cap(k,j)>0 & isempty(find(Ntmp==j))
            Ntmp(m)=j;
            m=m+1;
        end
    end
    N{k-1}=Ntmp;
end

%The above is correct;
%discharge vertices;
%for i=1:n_row-2
i=1;
while i<=n_row-2
%    i
    u=L(i);
    h_old=h(u);
    
    %discharge u;
    [m_r,m_c]=size(N{u-1});
    j=1;
    while e(u)>0
        if j>m_c
            %lift operation;
            cfu=cap(u,:)-f(u,:);
            vi=find(cfu==1);
            h(u)=1+min(h(vi));
            %lift ends;
            j=1;
        else
            v=N{u-1}(j);
            if cap(u,v)-f(u,v)>0 & h(u)==h(v)+1;
                %push operation;
				df=min(e(u),cap(u,v)-f(u,v));
				f(u,v)=f(u,v)+df;
				f(v,u)=-f(u,v);
				e(u)=e(u)-df;
				e(v)=e(v)+df;
                %push ends
            else
                j=j+1;
            end
        end
    end
    %discharge ends;

    if h(u)>h_old
        L=[u,L(1:i-1),L(i+1:n_row-2)];
        i=1;
    end
    i=i+1;
end

%etime(clock,t0)