function PP = ProjectPointOntoLine(P1,P2,P)
PP = P1 + dot(P-P1,P2-P1) / dot(P2-P1,P2-P1) * (P2-P1);
end