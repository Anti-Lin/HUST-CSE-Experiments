COVER='test2.jpeg';
STEGO='jsteg.jpeg';
MSG='1.txt';

Jsteg_simulation(COVER,STEGO,MSG);

function Jsteg_simulation(COVER,STEGO,MSG)
    fp=fopen(MSG,'r');
    [msg,len]=fread(fp,'ubit1');
    msg=msg';
%     msg����Ϣ�Ķ����ƣ�len����Ϣ����
    tic;
    try
        jobj=jpeg_read(COVER);
        DCT=jobj.coef_arrays{1};
        DCT2=DCT;
%         �õ�DCT
    catch
        error('ERROR(problem with the cover image)');
    end

    AC=numel(DCT)-numel(DCT(1:8:end,1:8:end));
%     ACϵ���ĸ���

    if(length(msg)>AC)
        error('ERROR(too long message)');
    end
    pos=true(size(DCT));
    pos(1:8:end,1:8:end)=false;
    pos=find(pos);
%     DCT�з�0Ԫ�ص�λ��

    j=1;
    for i=1:len
%         ��abs>1��ACϵ��
        while(abs(DCT(pos(j)))<=1)
            j=j+1;
            if(j>=AC)
                break;
            end
        end
%         Ƕ���pos(j)��ACϵ��
        if(j>=AC)
            break;
        end
        if(DCT(pos(j))>1)
            DCT2(pos(j))=DCT2(pos(j))-mod(DCT2(pos(j)),2)+msg(i);
        else
            DCT2(pos(j))=DCT2(pos(j))-mod(DCT2(pos(j))+1,2)+msg(i);
        end
        j=j+1;
        if(j>=AC)
            break;
        end
    end 
%     ֱ��ͼ
%     figure();
%     subplot(121);histogram(DCT);axis([-10,10,0,7*1e4]);title("Ƕ��ǰ");
%     subplot(122);histogram(DCT2);axis([-10,10,0,7*1e4]);title("Ƕ���");
%     ֵ������
%     figure();
%     subplot(121);histogram(DCT,(10:1:30));title("Ƕ��ǰ");
%     subplot(122);histogram(DCT2,(10:1:30));title("Ƕ���");
    try
        jobj.coef_arrays{1}=DCT2;
        jobj.optimize_coding=1;
        jpeg_write(jobj,STEGO);
    catch
        error('ERROR(problem with saving the stego image)')
    end
    fclose(fp);
    T=toc;
    fprintf('Ƕ��ʱ��:%4f��\n',T);
    fprintf('Ƕ�볤��:%d\n',len);
    
%     ͼƬ�Ա�
%     figure();
%     subplot(121);imshow(COVER);title("Ƕ��ǰ");
%     subplot(122);imshow(STEGO);title("Ƕ���");
end
