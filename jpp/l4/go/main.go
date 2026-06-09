package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"sync"
	"time"
)

type SyncPrint struct {
	mu sync.Mutex
}

func (sp *SyncPrint) Println(s string) {
	sp.mu.Lock()
	defer sp.mu.Unlock()
	fmt.Printf("[debug] %s\n", s)
}

type SharedData struct {
	finished_mu *sync.RWMutex
	finished    []bool
	forks       []chan bool
	print       *SyncPrint
}

func (sd *SharedData) Untag(id int) {
	defer sd.finished_mu.Unlock()
	sd.finished_mu.Lock()
	sd.finished[id] = true
}

func (sd *SharedData) AllFinished() bool {
	defer sd.finished_mu.RUnlock()
	sd.finished_mu.RLock()
	for _, b := range sd.finished {
		if !b {
			return false
		}
	}
	return true
}

type Stats struct {
	eatenMeals int
}

func getRandomDuration(min, max int64) time.Duration {
	return time.Duration(min + rand.Int63n(max-min+1))
}

func getEatingDuration() time.Duration {
	return getRandomDuration(100, 500)
}

func getThinkingDuration() time.Duration {
	return getRandomDuration(100, 500)
}

func eat(id int, sp *SyncPrint) {
	duration := getEatingDuration() * time.Microsecond
	sp.Println(fmt.Sprintf("%d is eating for %s", id, duration.String()))
	time.Sleep(duration)
}

func think(id int, sp *SyncPrint) {
	duration := getThinkingDuration() * time.Microsecond
	sp.Println(fmt.Sprintf("%d is thinking for %s", id, duration.String()))
	time.Sleep(duration)
}

func right(id, n int) int {
	i := (id + 1) % n
	if id == n-1 {
		i = (id + n - 1) % n
	}
	return i
}

func left(id, n int) int {
	i := (id + n - 1) % n
	if id == n-1 {
		i = (id + 1) % n
	}
	return i
}

func takeForks(id int, shared *SharedData) {
	n := len(shared.forks)
	<-shared.forks[left(id, n)]
	<-shared.forks[right(id, n)]
}

func putForks(id int, shared *SharedData) {
	n := len(shared.forks)
	shared.forks[left(id, n)] <- true
	shared.forks[right(id, n)] <- true
}

func philosopher(id, nMeals int, wg *sync.WaitGroup, shared *SharedData, stats *Stats) {
	defer wg.Done()
	eatenMeals := 0
	untagged := false

	for !shared.AllFinished() {
		think(id, shared.print)
		takeForks(id, shared)
		eat(id, shared.print)
		putForks(id, shared)

		eatenMeals++
		if !untagged && eatenMeals >= nMeals {
			shared.Untag(id)
			untagged = true
		}
	}

	stats.eatenMeals = eatenMeals
}

func parseArgs() (int, int, error) {
	if len(os.Args) < 3 {
		return 0, 0, fmt.Errorf("Usage: go run main.go <n_philosophers> <n_meals_per_philosopher>")
	}

	nPhilosophers, err := strconv.Atoi(os.Args[1])
	if err != nil {
		return 0, 0, fmt.Errorf("Invalid number of philosophers: %v", err)
	}

	nMeals, err := strconv.Atoi(os.Args[2])
	if err != nil {
		return 0, 0, fmt.Errorf("Invalid number of meals per philosopher: %v", err)
	}

	return nPhilosophers, nMeals, nil
}

func Min[T int](a, b T) T {
	var r T
	if a > b {
		r = b
	} else {
		r = a
	}
	return r
}

func Max[T int](a, b T) T {
	var r T
	if a > b {
		r = a
	} else {
		r = b
	}
	return r
}

func main() {
	nPhilosophers, nMeals, err := parseArgs()
	if err != nil {
		fmt.Println(err)
		return
	}

	shared := &SharedData{
		finished_mu: &sync.RWMutex{},
		finished:    make([]bool, nPhilosophers),
		forks:       make([]chan bool, nPhilosophers),
		print:       &SyncPrint{},
	}
	for i := range nPhilosophers {
		shared.finished[i] = false
		shared.forks[i] = make(chan bool, 1)
		shared.forks[i] <- true
	}

	wg := &sync.WaitGroup{}
	wg.Add(nPhilosophers)

	stats := make([]Stats, nPhilosophers)
	for i := range stats {
		go philosopher(i, nMeals, wg, shared, &stats[i])
	}

	wg.Wait()

	for _, ch := range shared.forks {
		close(ch)
	}

	total := 0
	min, max := math.MaxInt, 0
	avg := 0.0

	for _, s := range stats {
		total += s.eatenMeals
		min = Min(min, s.eatenMeals)
		max = Max(max, s.eatenMeals)
	}
	avg = float64(total) / float64(nPhilosophers)

	fmt.Println("\n---------- Results ----------")
	fmt.Printf("Total eaten meals: %d\n", total)
	fmt.Printf("Average meals per philo: %.1f\n", avg)
	fmt.Printf("Min-max eaten meals: min=%d max=%d\n", min, max)

	for i, s := range stats {
		fmt.Printf("\t[%d] meals eaten: %d\n", i, s.eatenMeals)
	}
}
