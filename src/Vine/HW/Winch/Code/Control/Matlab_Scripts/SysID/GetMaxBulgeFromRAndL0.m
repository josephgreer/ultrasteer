function D0 = GetMaxBulgeFromRAndL0(L0,R0,P,a,mode)
e0 = GetE0(L0,R0,P,a,mode);
phirm = GetPhiRandM(e0,L0,R0,P,a, mode);
phir = phirm(1);
D0 = 2*R0/cos(phir);
end