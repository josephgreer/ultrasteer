%% Represents polynomial poly = (p1(s), p2(s), p3(s))
%      poly = [a11 a12 a13;
%             a21 a22 a23;
%             ...
%             ad1 ad2 ad3];
%  where (a11^(d-1)+...+ad1, a12^(d-1)+...+ad2, a13^(d-1)+...+ad3)
%  return value = [p1(s) p2(s) p3s()]

classdef Poly3
    properties
        A
        N
    end
    methods
        function obj = Poly3(cof)
            obj.A = cof;
            obj.N = size(obj.A,1);
        end
        function d = degree(a)
            d = a.N-1;
        end
        function c = plus(a,b)
            m = a.degree()+1;
            n = b.degree()+1;
            N = max(m,n);
            polyA = [zeros(N-m,3); a.A];  polyB = [zeros(N-n,3); b.A];
            c = Poly3(polyA+polyB);
        end
        function v = eval(a,s)
            v = [polyval(a.A(:,1), s) polyval(a.A(:,2),s) polyval(a.A(:,3),s)];
        end
        function c = times(a,b)
            c = Poly3([conv(a.A(:,1),b.A(:,1)) conv(a.A(:,2),b.A(:,2)) conv(a.A(:,3),b.A(:,3))]);
        end
        function s = disp(a)
            s = '[';
            for j=1:3
                if j > 1
                    s = sprintf('%s, ', s);
                end
                if(a.N > 1)
                    s = sprintf('%s%d*s^%d', s, a(1).A(1,j), a(1).N-1);
                end
                for i=2:a(1).N-1
                    s  = sprintf('%s + %d*s^%d', s, a(1).A(i,j), a(1).N-i);
                end
                s = sprintf('%s + %d', s, a(1).A(a(1).N,j));
            end
            s = strcat(s,']');
            s
        end
        function s = deriv(a)
            % polyder will chop excess zeros off
            a1 = polyder(a.A(:,1))';
            a2 = polyder(a.A(:,2))';
            a3 = polyder(a.A(:,3))';
            N = max([length(a1) length(a2) length(a3)]);
            a1 = [zeros(N-length(a1),1);a1];
            a2 = [zeros(N-length(a2),1);a2];
            a3 = [zeros(N-length(a3),1);a3];
            s = Poly3([a1 a2 a3]);
        end
        % evaluate elements of frenet frame
        function [T N roc] = frenet(r,s)
            %%
            % T = r'/|r'| = r'/(|r'|^2)^(1/2)
            % N = T'/|T'|
            % Use quotient rule to calculate N in terms of square roots and
            % polynomials
            
            %first calculate polynomial derivatives
            rp = r.deriv();    % r'
            rpp = rp.deriv();  % r''
            rpsq = rp.*rp;  % (r'*r')
            rpmagsq = (rpsq.A(:,1)+rpsq.A(:,2)+rpsq.A(:,3))';  %|r'|^2
            rpmagsqp = polyder(rpmagsq);
            
            % now evaluate at desired point
            rp = rp.eval(s);
            rpp = rpp.eval(s);
            rpmagsq = polyval(rpmagsq,s);
            rpmagsqp = polyval(rpmagsqp,s);
            
            % T = r'/|r'|
            T = rp/sqrt((rp*rp'));
            
            % use quotient rule to calculate T' 
            Tp = (sqrt(rpmagsq)*rpp - 1/2*rp*(1/sqrt(rpmagsq))*rpmagsqp)/rpmagsq;
            N = Tp/sqrt(Tp*Tp'); % N = T'/|T'|
            
            % radius of curvature = |r'|^3/|r'xr''|
            rpcrossrpp = cross(rp,rpp);
            roc = (rpmagsq^(3/2))/(rpcrossrpp*rpcrossrpp')^(1/2);
            
        end
    end
    methods(Static)
        % returns Poly3 that goes through 3 vectors [x1 x2 x3] (given in a)
        function p = ThroughPoints(a)
            %solve p_i(0) = a(1,i)
            %solve p_i(0.5) = a(2,i)
            %solve p_i(1) = a(3,i) to get A
            A = [2 -4 2; -3 4 -1; 1 0 0];
            p = Poly3(A*a);
        end
    end
end