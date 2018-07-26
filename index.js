const { spawn } = require('child_process');

module.exports = {
	trainNew(layers, trainingSets) {
		const trainer = spawn(`${__dirname}/bin/trainer`);

		trainer.stdout.on("data", d => console.log(d, d.toString()));
		trainer.stderr.on("data", d => console.log(d, d.toString()));
		trainer.on("error", d => console.log(d, d.toString()));
		trainer.on("close", (...args) => console.log(...args));

		trainer.stdin.write(`${layers.length} ${layers.join(" ")}\n`);

		trainer.stdin.write(`${trainingSets.length}\n`);

		for(const trainingSet of trainingSets) {
			trainer.stdin.write(`${trainingSet[0].join(" ")}\n`);
			trainer.stdin.write(`${trainingSet[1].join(" ")}\n`);
		}
	}
};

module.exports.trainNew([ 2, 5, 2], [
	[ [ 0, 1 ], [ 1, 0 ] ],
	[ [ 1, 0 ], [ 0, 1 ] ],
	[ [ 0, 0 ], [ 1, 1 ] ],
	[ [ 1, 1 ], [ 0, 0 ] ]
]);
