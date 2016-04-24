function ims = MVCGetImage( camera_ids,mvcMgr )

ims = mvc_get_image(camera_ids,mvcMgr);
for i=1:length(ims)
    ims{i} = rot90(ims{i});
end

end

