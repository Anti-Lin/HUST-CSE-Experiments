COVER='test2.jpeg';
STEGO='f4.jpeg';
MSG='1.txt';
f4_simulation(COVER,STEGO,MSG);

function f4_simulation(COVER,STEGO,MSG)
    fp=fopen(MSG,'r');
    [msg,len]=fread(fp,'ubit1');
    msg=msg';
%     ��ʱ������
    tic;
    sjy=0;
    try
        jobj=jpeg_read(COVER);
        DCT=jobj.coef_arrays{1};
        DCT2=DCT;
    catch
        error('ERROR(problem with the cover image)');
    end

    AC=numel(DCT)-numel(DCT(1:8:end,1:8:end));

    if(length(msg)>AC)
        error('ERROR(too long message)');
    end
    
    pos=true(size(DCT));
    pos(1:8:end,1:8:end)=false;
    pos=find(pos);

    j=1;
    for i=1:len
        while(1)
		%     Ѱ�ҿ���Ƕ���ACϵ��
            if((DCT(pos(j))==1 && msg(i)==0) || (DCT(pos(j))==-1 && msg(i)==1) || (DCT(pos(j))==0))
                if(DCT(pos(j))~=0)
                    sjy=sjy+1;
                end
                DCT2(pos(j))=0;
                j=j+1;           
                if(j>=AC)
                    break;
                end
            else
                break;
            end
        end
%         Ƕ���pos(j)��ACϵ��
        if(j>=AC)
            break;
        end
        if(DCT(pos(j))>0 && msg(i)~=mod(DCT2(pos(j)),2))
            sjy=sjy+1;
            DCT2(pos(j))=DCT2(pos(j))-1;
        elseif(DCT(pos(j))<0 && msg(i)~=mod(DCT2(pos(j))+1,2))
            sjy=sjy+1;
            DCT2(pos(j))=DCT2(pos(j))+1;
        end
        j=j+1;
        if(j>=AC)
            break;
        end
    end
%     ֱ��ͼ
%     figure();
%     subplot(121);histogram(DCT);axis([-10,10,0,7*1e4]);title("f4Ƕ��ǰ");
%     subplot(122);histogram(DCT2);axis([-10,10,0,7*1e4]);title("f4Ƕ���");
%     ��ֵ������
%     figure();
%     subplot(121);histogram(DCT,(10:1:30));title("f4Ƕ��ǰ");
%     subplot(122);histogram(DCT2,(10:1:30));title("f4Ƕ���");
    try
        jobj.coef_arrays{1}=DCT2;
        jobj.optimize_coding=1;
        jpeg_write(jobj,STEGO);
    catch
        error('ERROR(problem with saving the stego image)')
    end
%     ͼƬ�Ա�
%     figure();
%     subplot(121);imshow(COVER);title("Ƕ��ǰ");
%     subplot(122);imshow(STEGO);title("Ƕ���");
    T=toc;
    fprintf('Ƕ��ʱ��:%4f��\n',T);
    fprintf('Ƕ����Ϣ:%d����\n',i);
    fprintf('�޸ĵ�ACϵ������:%d��\n',sjy);
    fclose(fp);
end