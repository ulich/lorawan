import React from "react";

export default class Fetch extends React.Component {

    state = {
        data: null
    }

    componentDidMount() {
        this.load()

        if (this.props.interval) {
            this.intervalJob = setInterval(this.load.bind(this), this.props.interval)
        }
    }

    componentWillUnmount() {
        if (this.intervalJob) {
            clearInterval(this.intervalJob)
        }
    }

    load() {
        fetch(this.props.url)
            .then(response => {
                return response.json()
            })
            .then(data => {
                this.setState({ data })
            })
    }

    render() {
        if (this.state.data) {
            return this.props.children({ data: this.state.data })
        }

        return null
    }
}