[X,map] = imread('1.png');

f=X(:,:,1);
imshow(f,[0 max(max(f))],'InitialMagnification','fit')

[M,N] = size(f);

%  for k=1:M
%  for l=1:N
%   
%   f(k,l) = (-1)^(k+l)*f(k,l);  
%       
%  end
%  end
% imshow(f,'InitialMagnification','fit')

F = fft2(f);
F = fftshift(F);       %�.1-2

Fla = log(abs(F));
%imshow(Fla,[min(min(F),max(max(F))],'InitialMagnification','fit')


% R = N/6;
% %����������------------------------------------------------------------------------
%   for k=1:M
%   for l=1:N
%            H=0; 
%   if( (k-M/2)^2 + (l-N/2)^2 < R^2 )
%            H=1; 
%   end
%   
%   F(k,l)= F(k,l)*H;
%     
%   end
%   end
  
funKai = zeros(M,N,'double');
funKaiROW = kaiser(M,30);
funKaiCOL = kaiser(N,30);
for k=1:M
  for l=1:N
      funKai(k,l) = (funKaiROW(k)*funKaiCOL(l));
      F(k,l)= F(k,l)*funKai(k,l);
  end
end

 
  Fmla = log(abs(F));
%imshow(Fmla,[-1 19],'InitialMagnification','fit');

F = ifftshift(F);
fil_f = ifft2(F);
fil_f = real(fil_f);

%  for k=1:M
%  for l=1:N
%   
%   fil_f(k,l) = (-1)^(k+l)*fil_f(k,l);  
%       
%  end
%  end

imshow(fil_f,[0 max(max(fil_f))],'InitialMagnification','fit')














