import React, { Component } from 'react';
import { Chart } from "react-google-charts";
import Fetch from './Fetch';

const url = `http://hansehack.fh-luebeck.de/channels/16/feed.json?key=${process.env.REACT_APP_READ_KEY}&results=100`

export default class App extends Component {

    render() {
        return (
            <div style={{ textAlign: 'center' }}>
                <Fetch url={url} interval={10000}>
                    {({ data }) => (
                        <div>
                            <Chart
                                chartType="Gauge"
                                data={[
                                    ["°C"],
                                    gaugeData(data)
                                ]}
                                height="400px"
                                style={{ display: 'inline-block' }}
                                options={{ yellowFrom: 25, yellowTo: 40, redFrom: 40, redTo: 100 }} />
                            
                            <Chart
                                chartType="AreaChart"
                                data={[
                                    ['Time', '°C'],
                                    ...historyData(data)
                                ]}
                                height="300px"
                                options={{
                                    title: 'History',
                                    vAxis: { title: '°C' },
                                    legend: { position: 'none' }
                                }}
                                // For tests
                                rootProps={{ 'data-testid': '1' }}
                                />
                        </div>
                    )}
                </Fetch>
            </div>
        );
    }
}

function gaugeData(data) {
    const lastFeed = data.feeds[data.feeds.length-1]
    return [
        Number(lastFeed.field1)
    ]
}

function historyData(data) {
    return data.feeds.map(f => {
        const date = new Date(f.created_at)
        return [date, Number(f.field1)]
    })
}