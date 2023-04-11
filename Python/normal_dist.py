import numpy as np
import scipy.stats as sps
import matplotlib.pyplot as plt


def pct_above_n_std_dev(mu, sd, n):
    # define the distribution
    dist = sps.norm(loc=mu, scale=sd)
    x = np.linspace(dist.ppf(.00001), dist.ppf(.99999))
    # Probability Density Function
    pdf = dist.pdf(x)
    # Cumulative Density Function
    cdf = dist.cdf(x)
    pct = 1 - dist.cdf(mu + n*sd)
    print(f"{pct:.10%}")
    return pct;


teensy_can = [310.4, 57.6];
teensy_canfd = [299.2, 0.8];

if __name__ == "__main__":
    x = pct_above_n_std_dev(310.4, 57.6, 6)
    pct_above_n_std_dev(299.2, 0.8, 1.5)
    pct_above_n_std_dev(1021.4, 117.7, 1.5)
    
'''
fig, axs = plt.subplots(1, 2, figsize=(12, 5))

axs[0].plot(x, pdf, color='k')
axs[0].fill_between(
    x[(x>=8)&(x<=17)],
    pdf[(x>=8)&(x<=17)],
    alpha=.25
)
axs[0].set(
    title='PDF'
)

axs[1].plot(x, cdf)
axs[1].axhline(dist.cdf(8), color='r', ls='--')
axs[1].axhline(dist.cdf(17), color='r', ls='--')
axs[1].set(
    title='CDF'
)
plt.show()
'''


#cdf_at_8 = dist.cdf(mu + 5*sd)

#cdf_at_17 = dist.cdf(17)

#cdf_between_8_17 = cdf_at_17 - cdf_at_8

#print(f"{cdf_between_8_17:.8%}")




