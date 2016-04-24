function mvcMgr = MVCInitCamera( camera_ids,widths,heights,hOffs,vOffs )
    if (size(camera_ids,1)~=1)
        error('camera_ids should be row vector');
    end
    camera_num = size(camera_ids,2);
    if (length(widths)==1)
        widths = ones(1,camera_num)*widths;
        heights = ones(1,camera_num)*heights;
        hOffs = ones(1,camera_num)*hOffs;
        vOffs = ones(1,camera_num)*vOffs;
    end
    if (camera_num~=size(widths,2) || camera_num~=size(heights,2) || camera_num~=size(hOffs,2) || camera_num~=size(vOffs,2))
        error('all the vector should be same size');
    end
    mvcMgr = mvc_init_camera(camera_ids,widths,heights,hOffs,vOffs );
end

