#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define f(x) (1 / (1 + fabs(x)))
#define random() fabs((double) rand() / (double) RAND_MAX * 2.0-1.0)

struct TrainingSet {
	double *inputs;
	double *outputs;
};

struct Neuron {
	double output;
	double bias;
	double *weights;
};

struct Layer {
	struct Neuron *neurons;
	size_t $neurons;
};

struct Net {
	struct Layer *layers;
	size_t $layers;
};

struct Net *net_create(uint32_t *layers, uint32_t $layers) {
	struct Net *net = calloc(sizeof(struct Net), 1);
	assert(net != NULL);

	net->$layers = $layers;
	net->layers = calloc(sizeof(struct Layer), net->$layers);
	assert(net->layers != NULL);

	for(size_t i = 0; i < $layers; i++) {
		struct Layer *layer = &net->layers[i];

		layer->$neurons = layers[i];
		layer->neurons = calloc(sizeof(struct Neuron), layer->$neurons);
		assert(layer->neurons != NULL);

		for(size_t j = 0; j < layer->$neurons; j++) {
			struct Neuron *neuron = &layer->neurons[j];

			neuron->bias = random();

			if(i > 0) {
				const size_t $weights = net->layers[i - 1].$neurons;
				neuron->weights = calloc(sizeof(double), $weights);

				for(size_t k = 0; k < $weights; k++) {
					neuron->weights[k] = random();
				}
			}
		}
	}

	return net;
}

void net_update(struct Net *net, double *inputs) {
	for(size_t i = 0; i < net->layers[0].$neurons; i++) {
		net->layers[0].neurons[i].output = inputs[i];
	}

	for(size_t i = 1; i < net->$layers; i++) {
		struct Layer *previousLayer = &net->layers[i - 1];
		struct Layer *layer = &net->layers[i];

		for(size_t i = 0; i < layer->$neurons; i++) {
			struct Neuron *neuron = &layer->neurons[i];
			double input = 0;

			for(size_t j = 0; j < previousLayer->$neurons; j++) {
				input += previousLayer->neurons[j].output * neuron->weights[j];
			}

			input += neuron->bias;

			neuron->output = f(input);
		}
	}
}

double _net_calculateError(struct Net *net, struct TrainingSet *trainingSets, size_t $trainingSets) {
	double error = 0;

	for(size_t j = 0; j < $trainingSets; j++) {
		struct TrainingSet *trainingSet = &trainingSets[j];

		net_update(net, trainingSet->inputs);

		struct Layer *outputLayer = &net->layers[net->$layers - 1];

		for(size_t i = 0; i < outputLayer->$neurons; i++) {
			struct Neuron *neuron = &outputLayer->neurons[i];

			double _error = trainingSet->outputs[i] - neuron->output;

			if(_error < 0)
				_error = _error * -1;

			error += _error;
		}
	}

	return error;
}

void net_train(struct Net *net, struct TrainingSet *trainingSets, size_t $trainingSets, size_t iterations, double step) {
	for(size_t i = 0; i < iterations; i++) {
		for(size_t j = 1; j < net->$layers; j++) {
			struct Layer *previousLayer = &net->layers[j - 1];
			struct Layer *layer = &net->layers[j];

			for(size_t k = 0; k < layer->$neurons; k++) {
				struct Neuron *neuron = &layer->neurons[k];

				for(size_t l = 0; l < previousLayer->$neurons; l++) {
					double weight = neuron->weights[l];
					double error[3];

					error[0] = _net_calculateError(net, trainingSets, $trainingSets);

					neuron->weights[l] = weight + step;
					error[1] = _net_calculateError(net, trainingSets, $trainingSets);

					neuron->weights[l] = weight - step;
					error[2] = _net_calculateError(net, trainingSets, $trainingSets);

					double lowestError = error[0];

					if(error[1] < lowestError)
						lowestError = error[1];

					if(error[2] < lowestError)
						lowestError = error[2];

					if(error[0] == lowestError) {
						neuron->weights[l] = weight;
					} else if(error[1] == lowestError) {
						neuron->weights[l] = weight + step;
					} else if(error[2] == lowestError) {
						neuron->weights[l] = weight - step;
					}
				}

				/*
				double bias = neuron->bias;
				double error[3];

				error[0] = _net_calculateError(net, trainingSets, $trainingSets);

				neuron->bias = bias + step;
				error[1] = _net_calculateError(net, trainingSets, $trainingSets);

				neuron->bias = bias - step;
				error[2] = _net_calculateError(net, trainingSets, $trainingSets);

				double lowestError = error[0];

				if(error[1] < lowestError)
					lowestError = error[1];

				if(error[2] < lowestError)
					lowestError = error[2];

				if(error[0] == lowestError) {
					neuron->bias = bias;
				} else if(error[1] == lowestError) {
					neuron->bias = bias + step;
				} else if(error[2] == lowestError) {
					neuron->bias = bias - step;
				}
				*/
			}
		}
	}
}

int main() {
	srand(time(NULL));

	uint32_t $layers;
	assert(fscanf(stdin, "%u", &$layers) > 0);

	assert($layers > 2);

	uint32_t *layers = calloc(sizeof(uint32_t), $layers);

	for(size_t i = 0; i < $layers; i++) {
		assert(fscanf(stdin, "%u", &layers[i]) > 0);
	}

	uint32_t $trainingSets;
	assert(fscanf(stdin, "%u", &$trainingSets) > 0);

	struct TrainingSet *trainingSets = calloc(sizeof(struct TrainingSet), $trainingSets);
	assert(trainingSets != NULL);

	for(size_t i = 0; i < $trainingSets; i++) {
		struct TrainingSet *trainingSet = &trainingSets[i];

		trainingSet->inputs = calloc(sizeof(double), layers[0]);

		for(size_t j = 0; j < layers[0]; j++) {
			assert(fscanf(stdin, "%lf", &trainingSet->inputs[0]) > 0);
		}

		trainingSet->outputs = calloc(sizeof(double), layers[$layers - 1]);

		for(size_t j = 0; j < layers[$layers - 1]; j++) {
			assert(fscanf(stdin, "%lf", &trainingSet->outputs[0]) > 0);
		}
	}

	struct Net *net = net_create(layers, $layers);
	net_train(net, trainingSets, $trainingSets, 10000, 0.01);

	double inputs0[] = { 1, 0 };

	net_update(net, inputs0);
	printf("%lf %lf ", net->layers[2].neurons[0].output, net->layers[2].neurons[1].output);

	double inputs1[] = { 0, 1 };


	net_update(net, inputs1);
	printf("%lf %lf\n", net->layers[2].neurons[0].output, net->layers[2].neurons[1].output);

	printf("%lf\n", random());
}
