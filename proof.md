# Fast Inverse Square Root on Double Precision
This Document proofs how double precision value is being similar to its inverse square root using same idea of the famous "Fast Inverse Square Root" algorithm.

## Representation of a real number on Double Prescision IEEE 754
### Sign Bit
It's the MSB of the number. Represents whether it is positive(0) or negative(1).

### Exponent
Next 11 bit of the number. Represents exponent using 2 as base number and has offset of 1023.

### Mantissa (Significand)
Last 52 bit including LSB. The intager value 1 is ignored.

### Conclusion
Actual value of a Double Prescision variable $d$ is:

<div align = "center">

$ d = (-1)^{s} \times 2^{E-1023} \times (1 + M) $

</div>

## Taking Logarithm
For a nonnegative double precision value $x$, by taking logarithm, it's value becomes

<div align = "center">

$ log_2(x) = log_2(d) $  
$ = log_2(2^{E-1023} \times (1+M)) $   
$ = E - 1023 + log_2(1+M) $   

</div>

Since

<div align = "center">

$ log_2(x+1) = x + \alpha (0 < \alpha < 1) $  
$ \rightarrow log_2(1+M) \approx M + \alpha$

</div>

Finally, the approximation of $log x$ of base $2$ is

<div align = "center">

$ log_2(x) = E_x - 1023 + M + \alpha $

</div>

## Evil floating point bit level hacking
```c
i = *(long long) &y;
```

By executing this line, the value of $I_x$ becomes

<div align = "center">

$ I_x = 2^{52} (E + M) $
$ \rightarrow (E + M) = \frac{I_x}{2^{52}} $

</div>


## "what the fuck?"
Let's begin from equation $ y = \frac{1}{\sqrt{x}} $.

<div align = "center">

$ y = \frac{1}{\sqrt{x}} $  
$ log_2(y) = log_2(\frac{1}{\sqrt{x}}) $  
$ log_2(y) = -\frac{1}{2} log_2(x) $  
$ E_y - 1023 + M_y + \alpha = -\frac{1}{2} (E_x - 1023 + M_x + \alpha) $  
$ \frac{I_y}{2^{52}} - 1023 + \alpha = -\frac{1}{2} (\frac{I_x}{2^{52}} - 1023 + \alpha) $  

$ \frac{I_y}{2^{52}} = -\frac{1}{2} \cdot \frac{I_x}{2^{52}} + \frac{3}{2} \cdot 1023 - \frac{3}{2} \cdot \alpha $  

$ I_y = \frac{3}{2} \cdot 2^{52} (1023 - \alpha) - \frac{1}{2}I_x $  
</div>

Therefore, $ \frac{3}{2} \cdot 2^{52} (1023 - \alpha) $ becomes the magic number, whilst the code ```-(i >> 1)``` is equivalent to $-\frac{1}{2}I_x$.

## Exact value of the magic number
It is well known that best number to approximate $ log(x) = x + \alpha $ is $\alpha \approx 0.043035$.
Subtitute to $\alpha$, the value is

<div align = "center">

$ \frac{3}{2} \cdot 2^{52} (1023 - \alpha) $   
$ 1.5 \times 4,503,599,627,370,496 \times (1,022.95965) $  
$ = 6,910,501,047,832,579,512.7296 $  
= 0x5FE6EC85E7DE30DA 

</div>

## Newton-Raphson Method

To evaluate a value more precisely, Newton-Raphson Method is being used.

<div align = "center">

$ y = \frac{1}{\sqrt{x}} $

</div>

To find the solution of $ f(y) $, square it and inverse it.

<div align = "center">

$ y^2 = \frac{1}{x} \Rightarrow \frac{1}{y^2} = x \Rightarrow \frac{1}{y^2}-x = 0 $  

</div>

Therefore, the function of the formula we have to solve is

<div align = "center">

$ f(y) = \frac{1}{y^2}-x $

</div>

and the equation in form of Newton-Raphson Method is

<div align = "center">

$y_{n+1} = y_n - \frac{f(y_n)}{f'(y_n)}$  

</div>


By differentiate the function for y, the $f'(y)$ is  

<div align = "center">

$ f'(y) = -2y^{-3} = \frac{2}{y^3} $

</div>

Now, subtituting $f(y)$ and $f'(y)$ into the formula

<div align = "center">

$ y_{n+1} = y_{n} - \frac{\frac{1}{y^2_n}-x}{-\frac{2}{y^3_n}} $

</div>

Clean up.

<div align = "center">

$ y_{n+1} = y_n + \left( \frac{1}{y^2_n} - x \right) \cdot \frac{y^3_n}{2} $  
$ y_{n+1} = y_n + \left( \frac{y^3_n}{2y^2_n} - \frac{x \cdot y^3_n}{2} \right) $  
$ y_{n+1} = y_n + \frac{1}{2} y_n - \frac{1}{2} x \cdot y^3_n $   
$ y_{n+1} = y_n \cdot \left( 1.5 - \left(\frac{1}{2}x \right) \cdot y^2_n \right) $  

</div>


