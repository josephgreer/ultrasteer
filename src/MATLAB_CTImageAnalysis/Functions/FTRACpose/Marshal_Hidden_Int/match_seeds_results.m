function percentage = match_seeds_results

percentage = [0 0];

cmb = [];

% for i = 1:6
%     for j = 1:6
%         if (i ~= j)
%             for k = 1:6
%                 if (i ~= j && j ~= k && i ~= k)
%                     for l = 1:6
%                         if (i ~= j && j ~= k && i ~= k && i ~= l && j ~= l && k ~= l)
%                             [i j k l]
%                             percentage = percentage + match_seeds_hidden([i j k l]);
%                         end
%                     end
%                 end
%             end
%         end
%     end
% end

for i = 1:6
    for j = 1:6
        if (i ~= j)
            for k = 1:6
                if (i ~= j && j ~= k && i ~= k)
                    [i j k]
                    percentage = percentage + match_seeds_hidden([i j k]);
                end
            end
        end
    end
end

% cmb = [1 2; 3 1; 4 1; 5 1; 6 1; 3 2; 4 2; 5 2; 6 2; 4 3; 3 5; 6 3; 4 5; 6 4; 6 5];
% % a = 0
% for i = 1:15
%     for j = 1:6
%         if (cmb(i, 1) ~= j && cmb(i, 2) ~= j)
%             for k = 1:6
%                 if (cmb(i, 1) ~= k && cmb(i, 2) ~= k && j ~= k)
%                     for l = 1:6
%                         if (cmb(i, 1) ~= l && cmb(i, 2) ~= l && l ~= k && l ~= j)
%                             for m = 1:6
%                                 if (cmb(i, 1) ~= m && cmb(i, 2) ~= m && m ~= k && m ~= j && m ~= l)
% %                                     a = a + 1;
%                                     [cmb(i,:) j k l m]
%                                     percentage = percentage + match_seeds_hidden([cmb(i, :) j k l m]);
%                                 end
%                             end
%                         end
%                     end
%                 end                    
%             end
%         end
%     end
% end
% 
% % a
% % percentage = percentage + match_seeds_hidden([6 5 4]);