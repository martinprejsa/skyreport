# Skyreport
Collects data from [SQM-LE](http://unihedron.com/projects/sqm-le/) and [Renkforce WH2600](https://www.conrad.com/en/p/renkforce-wh2600-wireless-digital-weather-station-forecasts-for-12-to-24-hours-max-number-of-sensors-2-1267654.html?refresh=true) in specified time intervals and exports them to into CSV tables, or sends them via NxAgent to a NetXMS server.

Written in `C17` for `GNU/Linux` for compiler `GCC >= 11`

## Configuration
Skyreport looks for the `skyreport.conf` file it's working directory.
| Key | Default value | Description |
| ---------- | -----| ----------- |
| sqm-le-port | 10001 | SQM-LE port |
| sqm-le-address | - | SQM-LE address |
| wh2600-port | 8080 | WH2600 http server port |
| wh2600-timeout| 15| WH2600 http server timeout |
| log-dir | /var/log/skyreport | Log files directory |
| nxapush-bin | nxapush | Path to the nxapush binary |
| sample-count| 3 | Amount of samples per run |
| sample-period | 3 hours (60 * 60 * 3) | Sample period in seconds |
| enable-log | 1 | Enable file log (1 or 0)|
| enable-netxms| 0 | Enable netxms log |

## NetXMS DCI
Uses `nxapush` command to send the collected data.
| DCI Metric | Unit | Description |
| ---------- | -----| ----------- |
| skyreport-surface-brightness | mag/arcsec^2 | Surface brightness (SQM-LE)|
| skyreport-humidity | % | Air humidity (WH2600)|
| skyreport-temperature | °C | Air temperature (WH2600)|
| skyreport-fail | String | "true" if the skyreport software didn't run correctly, "false" otherwise|
| skyreport-fail-message | String | Failure message |