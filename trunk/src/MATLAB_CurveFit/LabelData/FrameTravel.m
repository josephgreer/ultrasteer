function travel = FrameTravel(header, mpp, gps1, gps2)
ul = header.ul;
br = header.br;
bl = [header.ul(1) header.br(2)];
ur = [header.br(1) header.ul(2)];

ul1 = MapImageCoordToWorldCoord(ul, header, mpp, gps1);
bl1 = MapImageCoordToWorldCoord(bl, header, mpp, gps1);
br1 = MapImageCoordToWorldCoord(br, header, mpp, gps1);
ur1 = MapImageCoordToWorldCoord(ur, header, mpp, gps1);

ul2 = MapImageCoordToWorldCoord(ul, header, mpp, gps2);
bl2 = MapImageCoordToWorldCoord(bl, header, mpp, gps2);
br2 = MapImageCoordToWorldCoord(br, header, mpp, gps2);
ur2 = MapImageCoordToWorldCoord(ur, header, mpp, gps2);

travel = norm(ul1-ul2)+norm(bl1-bl2)+norm(br1-br2)+norm(ur2-ur1);
end