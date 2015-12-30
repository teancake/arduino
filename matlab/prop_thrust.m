%% prop_thrust: Calculate the estimated thrust of a propeller 
% Outputs: 
%   F - thrust in Newton [N]
%   Fg - thrust in grams [g]
% Inputs:
%   d - propeller diameter [inch]
%   rpm - propeller rotations per minute
%   pitch - prop pitch [inch], 0.6 by default
%   V0 - vertical/forward speed [m/s], 0 by default
% Example:  [F, Fg] = prop_thrust(2.2,26600)
%	    [F, Fg] = prop_thrust(75/25.4, 17400, 75/25.4*0.6, 1)
% Reference: http://www.electricrcaircraftguy.com/2013/09/propeller-static-dynamic-thrust-equation.html
% 
% Xiaoke Yang <das.xiaoke@hotmail.com>
%% 
function [F, Fg] = prop_thrust (d, rpm, pitch, V0)
if (nargin < 2)
    disp('Insufficient number of arguments. Syntax: prop_thrust(d, rpm) or prop_thrust(d, rpm, pitch, V0). Type help prop_thrust for more information.');
    return;
end 
if (nargin < 3)
    pitch = 0.6*d;
end
if (nargin < 4)
    V0 = 0;
end
tmp = rpm*0.0254*pitch*1/60;
F = 1.225*pi*(0.0254*d)^2/4*(tmp^2-tmp*V0)*(d/3.29546/pitch)^1.5;
Fg = F/9.81*1000;
endfunction
