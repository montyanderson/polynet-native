const { spawn } = require('child_process');

module.exports = {
	async trainNew(layers, trainingSets, { step = 0.001, iterations = 10000 } = {}) {
		const trainer = spawn(`${__dirname}/bin/trainer`);

		const net = [];

		for(let i = 0; i < layers.length; i++) {
			const layer = [];

			for(let j = 0; j < layers[i]; j++) {
				layer.push({
					input: 0,
					bias: 0,
					output: 0,
					weights: []
				});
			}

			net.push(layer);
		}

		trainer.stderr.on("data", d => console.log(d, d.toString()));

		trainer.stdin.write(`${layers.length}\n${layers.join("\n")}\n`);

		trainer.stdin.write(`${trainingSets.length}\n`);

		for(const trainingSet of trainingSets) {
			trainer.stdin.write(`${trainingSet[0].join("\n")}\n`);
			trainer.stdin.write(`${trainingSet[1].join("\n")}\n`);
		}

		trainer.stdin.write(`${step}\n${iterations}\n`);

		let output = "";

		trainer.stdout.on("data", data => {
			output += data;
		});

		await new Promise((resolve, reject) => {
			trainer.on("error", reject);
			trainer.on("exit", code => {
				code === 0 ? resolve() : reject(new Error(code))
			});
		});

		const outputs = output.split("\n");

		for(const layer of net) {
			if(net.indexOf(layer) === 0)
				continue;

			const previousLayer = net[net.indexOf(layer) - 1];

			for(const neuron of layer) {
				neuron.bias = Number(outputs.shift());

				for(let i = 0; i < previousLayer.length; i++)
					neuron.weights[i] = Number(outputs.shift());
			}
		}

		return net;
	}
};
