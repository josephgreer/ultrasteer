function [s,T,c_T,f_T]=match_tree(f,c)
%find the initial spaning tree with a flow n

[n_node,m_node]=size(f);
%start with S={1};
s=1;
T=zeros(1,n_node);
%for each u~=v, let closest_u=v, and d_u=c(u,v) or 100;
list_u=ones(n_node-1,3)*1000;
list_u(:,1)=(2:n_node)';
list_u(:,2)=ones(n_node-1,1);
for i=1:n_node-1
    if f(list_u(i,1),1)~=0
        if c(list_u(i,1),1)<list_u(i,3)
            list_u(i,3)=c(list_u(i,1),1);
        end
        if c(1,list_u(i,1))<list_u(i,3)
            list_u(i,3)=c(1,list_u(i,1));
        end
    end
end

c_T=ones(n_node,m_node)*1000;
f_T=zeros(n_node,m_node)*1000;
while length(s)<n_node
    %find min cost u;
    [y,i]=min(list_u(:,3));
    %add min cost u arc to tree;
    c_T(list_u(i,1),list_u(i,2))=c(list_u(i,1),list_u(i,2));
    c_T(list_u(i,2),list_u(i,1))=c(list_u(i,2),list_u(i,1));
    f_T(list_u(i,1),list_u(i,2))=f(list_u(i,1),list_u(i,2));
    f_T(list_u(i,2),list_u(i,1))=f(list_u(i,2),list_u(i,1));
    s=[s,list_u(i,1)];
    T(list_u(i,1))=list_u(i,2);
    n_s=length(s);
    %update list_u;
    n_u=length(list_u(:,1));
    utmp=[list_u(1:i-1,1);list_u(i+1:n_u,1)];
    list_u=ones(n_u-1,3)*1000;
    list_u(:,1)=utmp;
    for i=1:n_u-1
        for j=1:n_s
            if f(list_u(i,1),s(j))~=0
                if c(list_u(i,1),s(j))<list_u(i,3)
                    list_u(i,3)=c(list_u(i,1),s(j));
                    list_u(i,2)=s(j);
                end
                if c(s(j),list_u(i,1))<list_u(i,3)
                    list_u(i,3)=c(s(j),list_u(i,1));
                    list_u(i,2)=s(j);
                end
            end
        end
    end
end
