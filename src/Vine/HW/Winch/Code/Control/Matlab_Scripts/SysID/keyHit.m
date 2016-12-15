function keyHit()
currChar = get(gcbf,'CurrentCharacter');
global state;
global i;
global mark;
global run;
switch currChar
    case 'p'
        if(strcmpi(state,'play'))
            state = 'pause';
        else
            state = 'play'
        end
    case 'q'
        state = 'pause';
        i = max(i-1,1);
    case 'z'
        state = 'pause';
        i = max(i-100,1);
    case 'x'
        state = 'pause'
        i = max(i+100,1)
    case 'w'
        state = 'pause';
        i = max(i+1,1);
    case 'm'
        mark = true;
    case 'r'
        run = false;
end
end