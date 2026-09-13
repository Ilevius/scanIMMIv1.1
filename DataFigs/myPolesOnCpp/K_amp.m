close all;
clc;
clear;
% LC = IMMIcolors();

h = 1;

waveguide = 'LiNbE0E90E0H0.32'; 

% mysurf = load("..\data\"+waveguide+"\K_amplitude.txt");
mysurf = load("K_amplitude.txt");
x = mysurf(:,1)*h; y = mysurf(:,2); z = mysurf(:,3);    
    
% dp = load("..\data\"+waveguide+"\DotPoles.txt");
dp = load("DotPoles.txt");

% x = mysurf(:,1)*h; y = mysurf(:,2); z = (mysurf(:,3));
ave = mean(z);
for i = 1:max(size(z))
    if z(i) > 2.5*ave
         z(i) = 2.5*ave;
    end
end
z = -(z);

xu = unique(x); yu = unique(y);
x_number = size(xu, 1);
y_number = size(yu, 1);
Z = zeros(x_number, x_number);
 for k = 1:x_number
     i = (k-1)*x_number+1;
     Z(:,k) = z(i:x_number*k, 1);
 end
[Y,X] = meshgrid(yu,xu);


IMMI2024(220 , 120, 10, 1, 5);
s = pcolor(X,Y,Z);
colormap(summer)
s.FaceColor = 'flat';
s.EdgeColor = 'none';
hold on;
plot(dp(:,1)*h, dp(:,2), '.r');



% ylim([0.2 3]); xlim([0.02e6 6e6]);
% xticks([ 0 1 2 3 4 5]); yticks([5 10 15]);
xlabel('$fh$, km/s', 'Interpreter', 'Latex'); 
ylabel('$\alpha, mm^{-1}$', 'Interpreter', 'LaTex');
grid on;
% set(gcf,'renderer','painters');


