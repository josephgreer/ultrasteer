function params = loadParamsFullState(params, basePath, method)
path = strcat(basePath, 'paramsFullState.dat');
f = fopen(path, 'r');
while(~feof(f))
    str = fgetl(f);
    splits = strsplit(str, {' ', ','});
    
    [params worked] = loadParamsGeneral(params, splits);
    if(~worked)
        if(strcmpi(method, 'FullState'))
            params = loadParamsFullState(params, splits);
        else
            params = loadParamsMarginalized(params, splits);
        end
    end
end
end