function params = loadParams(params, basePath, method)
isMarg = strcmpi(method, 'Marginalized');
if(isMarg)
    path = strcat(basePath, 'paramsMarginalized.dat');
else
    path = strcat(basePath, 'paramsFullState.dat');
end
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