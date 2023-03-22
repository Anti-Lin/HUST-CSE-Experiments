COVER='test2.jpeg';
STEGO='f5.jpeg';
MSG='1.txt';

f5_simulation(COVER,STEGO,MSG);

function f5_simulation(COVER,STEGO,MSG)
    fp=fopen(MSG,'r');
    [msg,len]=fread(fp,'ubit1');
    msg=msg';
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

    i=1;
    j=1;
    k=1;
    thebit=1;
    a=[0,0,0];
    realpos=[0,0,0];
    while(i<=len)
        while(1)
		%     Ѱ�ҿ���Ƕ���ACϵ��
            if(DCT(pos(j))==0)
                j=j+1;
                if(j>=AC)
                    break;
                end
            else
		%     ��ACϵ����LSB�洢������a��ֱ����3��
                realpos(k)=pos(j);
                if(DCT(pos(j))>0)
                    a(k)=mod(DCT(pos(j)),2);
                else
                    a(k)=mod(DCT(pos(j))+1,2);
                end
                k=k+1;
                j=j+1;
                if(k==4)
                    k=k-3;
                    break;
                end                
            end
        end
        if(j>=AC)
            break;
        end
%         Ƕ��������DCTϵ���е���һ��
        if(xor(a(1),a(2))~=msg(i) && xor(a(2),a(3))==msg(i+1))
            sjy=sjy+1;
            thebit=1;
        elseif(xor(a(1),a(2))~=msg(i) && xor(a(2),a(3))~=msg(i+1))
            sjy=sjy+1;
            thebit=2;
        elseif(xor(a(1),a(2))==msg(i) && xor(a(2),a(3))~=msg(i+1))
            sjy=sjy+1;
            thebit=3;
        else
            k=1;
            i=i+2;
            continue;
        end
%         ��ʼǶ��
        if( DCT(realpos(thebit))>0 )
            DCT2(realpos(thebit))=DCT2(realpos(thebit))-1;
        elseif( DCT(realpos(thebit))<0)
             DCT2(realpos(thebit))=DCT2(realpos(thebit))+1;
        end
%         Ƕ��ʧ��
       if(DCT2(realpos(thebit))==0)
           k=3;
           if(thebit==2)
               a(2)=a(3);
               realpos(2)=realpos(3);
           end
           if(thebit==1)
              a(1)=a(2);
              realpos(1)=realpos(2);
              a(2)=a(3);
              realpos(2)=realpos(3);
           end
       else
%            Ƕ��ɹ�
           k=1;
           i=i+2;
       end
    end
%     ֱ��ͼ
%     figure();
%     subplot(121);histogram(DCT);axis([-10,10,0,7*1e4]);title("f5Ƕ��ǰ");
%     subplot(122);histogram(DCT2);axis([-10,10,0,7*1e4]);title("f5Ƕ���");
%     ֵ������
%     figure();
%     subplot(121);histogram(DCT,(10:1:30));title("f5Ƕ��ǰ");
%     subplot(122);histogram(DCT2,(10:1:30));title("f5Ƕ���");
    try
        jobj.coef_arrays{1}=DCT2;
        jobj.optimize_coding=1;
        jpeg_write(jobj,STEGO);
    catch
        error('ERROR(problem with saving the stego image)')
    end
    T=toc;
    fprintf('Ƕ��ʱ��:%4f��\n',T);
    fprintf('Ƕ����Ϣ:%d����\n',i-1);
    fprintf('�޸ĵ�ACϵ������:%d��\n',sjy);
    fclose(fp);
end